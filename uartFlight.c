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

#define DEBUG 1

#include <debug.h>
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

/** \example uartFlight.c */

#define MAXANGLE 45
#define ANGLESTEP 10
#define MAXMOTOR 255
#define MOTORSTEP 10
#define QUADFREQ 100
#define STATUSFREQ 10

#define QUADDELAY (1000 / QUADFREQ)
#define STATUSDELAY (1000 / STATUSFREQ)

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
void silent(void);

char PROGMEM motorToggleString[] = "Motor On/Off";
char PROGMEM motorUpString[] = "Up";
char PROGMEM motorDownString[] = "Down";
char PROGMEM motorLeftString[] = "Left";
char PROGMEM motorRightString[] = "Right";
char PROGMEM motorForwardString[] = "Forwards";
char PROGMEM motorBackwardString[] = "Backwards";
char PROGMEM controlToggleString[] = "Toggle PID";
char PROGMEM parameterChangeString[] = "Change PID Params";
char PROGMEM zeroString[] = "Angles to Zero";
char PROGMEM silentString[] = "Toggle Status Output";

#define STATE_MOTOR (1 << 0) // 1 -> Motor On
#define STATE_PID (1 << 1) // 1 -> PID enabled
#define STATE_OUTPUT (1 << 2) // 1 -> No Status Output
uint8_t state = 0;

uint8_t speed = 10;
int16_t targetRoll = 0;
int16_t targetPitch = 0;

uint32_t sumFlightTask = 0, sumFlightCount = 0;

int main(void) {
    xyInit();
    pidInit();
    motorInit();
    orientationInit();

    debugPrint("Initialized Hardware");

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
    addMenuCommand('z', zeroString, &zeroOrientation);
    addMenuCommand('o', silentString, &silent);

    xyLed(LED_RED, LED_OFF);
    xyLed(LED_GREEN, LED_ON);

    debugPrint("Starting Tasks");

    for(;;) {
        tasks();
    }

    return 0;
}

void flightTask(void) {
    static time_t last = 100; // Don't begin immediately
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

        uint32_t diff = getSystemTime() - last;
        if (++sumFlightCount >= QUADFREQ) {
            sumFlightCount = 1;
            sumFlightTask = diff;
        } else {
            sumFlightTask += diff;
        }
    }
}

void statusTask(void) {
    static time_t last = 100; // Don't begin immediately
    static uint32_t lastDuration = 0;
    if (((getSystemTime() - last) >= STATUSDELAY) && (!(state & STATE_OUTPUT))) {
        last = getSystemTime();
        printf("q%li %li\n", sumFlightTask / sumFlightCount, lastDuration);
        printf("r%.2f %.2f\n", o_pids[0].intMin, o_pids[0].intMax);
        printf("s%.2f %.2f\n", o_pids[0].outMin, o_pids[0].outMax);
        printf("t%.3f %.3f %.3f\n", o_pids[0].kp, o_pids[0].ki, o_pids[0].kd);
        printf("u%.2f %.2f\n", o_output[PITCH], o_output[ROLL]);
        printf("v%i %i %i %i\n", motorSpeed[0], motorSpeed[1], motorSpeed[2], motorSpeed[3]);
        printf("w%.2f\n", orientation.pitch);
        printf("x%.2f\n", orientation.roll);
        printf("y%.2f\n", orientation.yaw);
        printf("z%.2f\n", getVoltage());
        lastDuration = getSystemTime() - last;
    }
}

void controlToggle(void) {
    if (state & STATE_PID) {
        state &= ~STATE_PID;
        printf("PID Off!\n");
    } else {
        state |= STATE_PID;
        printf("PID On!\n");
    }
}

void motorToggle(void) {
    if (state & STATE_MOTOR) {
        state &= ~STATE_MOTOR;
        baseSpeed = 0;
        printf("Motor Off!\n");
    } else {
        state |= STATE_MOTOR;
        baseSpeed = speed = 10;
        printf("Motor On!\n");
    }
}

void motorUp(void) {
    if (speed <= (MAXMOTOR - MOTORSTEP)) {
        if (state & STATE_MOTOR) {
            speed += MOTORSTEP;
            baseSpeed = speed;
            printf("Throttle up to %i\n", speed);
        }
    }
}

void motorDown(void) {
    if (speed >= MOTORSTEP) {
        if (state & STATE_MOTOR) {
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

void silent(void) {
    if (state & STATE_OUTPUT) {
        // Currently disabled, bit set
        state &= ~STATE_OUTPUT; // Unset Bit
    } else {
        // Currently enabled
        state |= STATE_OUTPUT; // Set Bit
    }
}
