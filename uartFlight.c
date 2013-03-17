/*
 * uartFlight.c
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <tasks.h>
#include <error.h>
#include <xycontrol.h>
#include <time.h>
#include <uartMenu.h>
#include <serial.h>
#include <acc.h>
#include <gyro.h>
#include <mag.h>
#include <motor.h>
#include <orientation.h>
#include <pid.h>
#include <set.h>

#define MAXANGLE 45
#define ANGLESTEP 10
#define MAXMOTOR 255
#define MOTORSTEP 10
#define QUADFREQ 100
#define STATUSFREQ 5

#define QUADDELAY (1000 / QUADFREQ)
#define STATUSDELAY (1000 / STATUSFREQ)

#define QUADDELAY_ERR (QUADDELAY - 2)
#define STATUSDELAY_ERR (STATUSDELAY / 2)

void flightTask(void);
void statusTask(void);
void controlToggle(void);
void motorToggle(void);
void motorUp(void);
void motorDown(void);
void motorForward(void);
void motorBackward(void);
void motorLeft(void);
void motorRight(void);
void parameterChange(void);

char motorToggleString[] PROGMEM = "Motor On/Off";
char motorUpString[] PROGMEM = "Up";
char motorDownString[] PROGMEM = "Down";
char motorLeftString[] PROGMEM = "Left";
char motorRightString[] PROGMEM = "Right";
char motorForwardString[] PROGMEM = "Forwards";
char motorBackwardString[] PROGMEM = "Backwards";
char controlToggleString[] PROGMEM = "Toggle PID";
char parameterChangeString[] PROGMEM = "Change PID Params";

uint8_t state = 0; // Bit 0: Motor, Bit 1: PID
uint8_t speed = 10;
int16_t targetRoll = 0;
int16_t targetPitch = 0;

int main(void) {
    xyInit();
    pidInit();
    motorInit();
    orientationInit();

    addTask(&flightTask);
    addTask(&statusTask);

    addMenuCommand('m', motorToggleString, &motorToggle);
    addMenuCommand('w', motorForwardString, &motorForward);
    addMenuCommand('a', motorLeftString, &motorLeft);
    addMenuCommand('s', motorBackwardString, &motorBackward);
    addMenuCommand('d', motorRightString, &motorRight);
    addMenuCommand('x', motorUpString, &motorUp);
    addMenuCommand('y', motorDownString, &motorDown);
    addMenuCommand('p', controlToggleString, &controlToggle);
    addMenuCommand('n', parameterChangeString, &parameterChange);

    xyLed(LED_ALL, LED_ON);

    for(;;) {
        tasks();
    }

    return 0;
}

void flightTask(void) {
    static time_t last = 100; // Don't begin immediately
    static uint8_t anglesCorrected = 0;
    if ((getSystemTime() - last) >= QUADDELAY) {
        last = getSystemTime();
        Error e = orientationTask();
        REPORTERROR(e);
        if (state & 0x02) {
            pidTask();
        } else {
            o_output[0] = o_output[1] = 0;
        }
        setTask();
        motorTask();
        if (anglesCorrected < 25) {
            anglesCorrected++;
            if (anglesCorrected == 25) {
                zeroOrientation();
            }
        }
        long int diff = getSystemTime() - last;
        if (diff >= (QUADDELAY_ERR)) {
            printf("Flight Task took %lims!\n", diff);
        }
    }
}

void statusTask(void) {
    static time_t last = 100; // Don't begin immediately
    if ((getSystemTime() - last) >= STATUSDELAY) {
        last = getSystemTime();
        printf("r%.2f %.2f\n", o_pids[0].intMin, o_pids[0].intMax);
        printf("s%.2f %.2f\n", o_pids[0].outMin, o_pids[0].outMax);
        printf("t%.3f %.3f %.3f\n", o_pids[0].kp, o_pids[0].ki, o_pids[0].kd);
        printf("u%.2f %.2f\n", o_output[PITCH], o_output[ROLL]);
        printf("v%i %i %i %i\n", motorSpeed[0], motorSpeed[1], motorSpeed[2], motorSpeed[3]);
        printf("w%.2f\n", orientation.pitch);
        printf("x%.2f\n", orientation.roll);
        printf("y%.2f\n", orientation.yaw);
        printf("z%.2f\n", getVoltage());
        long int diff = getSystemTime() - last;
        if (diff >= (STATUSDELAY_ERR)) {
            printf("Status Task took %lims!\n", diff);
        }
    }
}

void controlToggle(void) {
    if (state & 0x02) {
        state &= ~0x02;
        printf("PID Off!\n");
    } else {
        state |= 0x02;
        printf("PID On!\n");
    }
}

void motorToggle(void) {
    if (state & 0x01) {
        state &= ~0x01;
        baseSpeed = 0;
        printf("Motor Off!\n");
    } else {
        state |= 0x01;
        baseSpeed = speed = 10;
        printf("Motor On!\n");
    }
}

void motorUp(void) {
    if (speed <= (MAXMOTOR - MOTORSTEP)) {
        if (state & 0x01) {
            speed += MOTORSTEP;
            baseSpeed = speed;
            printf("Throttle up to %i\n", speed);
        }
    }
}

void motorDown(void) {
    if (speed >= MOTORSTEP) {
        if (state & 0x01) {
            speed -= MOTORSTEP;
            baseSpeed = speed;
            printf("Throttle down to %i\n", speed);
        }
    }
}

void motorForward(void) {
    if (targetPitch >= (-1 * (MAXANGLE - ANGLESTEP))) {
        targetPitch -= ANGLESTEP;
        o_should[PITCH] = targetPitch;
        printf("Pitch Forward %i\n", targetPitch);
    }
}

void motorBackward(void) {
    if (targetPitch <= (MAXANGLE - ANGLESTEP)) {
        targetPitch += ANGLESTEP;
        o_should[PITCH] = targetPitch;
        printf("Pitch Backwards %i\n", targetPitch);
    }
}

void motorLeft(void) {
    if (targetRoll <= (MAXANGLE - ANGLESTEP)) {
        targetRoll += ANGLESTEP;
        o_should[ROLL] = targetRoll;
        printf("Roll Left %i\n", targetRoll);
    }
}

void motorRight(void) {
    if (targetRoll >= (-1 * (MAXANGLE - ANGLESTEP))) {
        targetRoll -= ANGLESTEP;
        o_should[ROLL] = targetRoll;
        printf("Roll Right %i\n", targetRoll);
    }
}

void parameterChange(void) {
    double p, i, d, min, max, iMin, iMax;
    int c = scanf("%lf %lf %lf %lf %lf %lf %lf", &p, &i, &d, &min, &max, &iMin, &iMax);
    if (c == 7) {
        pidSet(&o_pids[0], p, i, d, min, max, iMin, iMax);
        pidSet(&o_pids[1], p, i, d, min, max, iMin, iMax);
    } else {
        printf("Only got %i (%lf %lf %lf %lf %lf %lf %lf)!\n", c, p, i, d, min, max, iMin, iMax);
    }
}
