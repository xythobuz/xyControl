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

#define QUADFREQ 10
#define QUADDELAY (1000 / QUADFREQ)

#define STATUSFREQ 5
#define STATUSDELAY (1000 / STATUSFREQ)

void flightTask(void);
void statusTask(void);
void motorToggle(void);
void motorUp(void);
void motorDown(void);
void motorForward(void);
void motorBackward(void);
void motorLeft(void);
void motorRight(void);
void printRaw(void);

char motorToggleString[] PROGMEM = "Motor On/Off";
char motorUpString[] PROGMEM = "Up";
char motorDownString[] PROGMEM = "Down";
char motorLeftString[] PROGMEM = "Left";
char motorRightString[] PROGMEM = "Right";
char motorForwardString[] PROGMEM = "Forwards";
char motorBackwardString[] PROGMEM = "Backwards";
char sensorString[] PROGMEM = "Raw Sensor Data";

uint8_t motorState = 0;
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
    addMenuCommand('r', sensorString, &printRaw);

    xyLed(LED_ALL, LED_ON);

    for(;;) {
        tasks();
    }

    return 0;
}

void flightTask(void) {
    static time_t last = 0;
    if ((getSystemTime() - last) >= QUADDELAY) {
        orientationTask();
        pidTask();
        setTask();
        motorTask();
        last = getSystemTime();
    }
}

void statusTask(void) {
    static time_t last = 0;
    if ((getSystemTime() - last) >= STATUSDELAY) {
        printf("v%i %i %i %i\n", motorSpeed[0], motorSpeed[1], motorSpeed[2], motorSpeed[3]);
        printf("w%f\n", orientation.pitch);
        printf("x%f\n", orientation.roll);
        printf("y%f\n", orientation.yaw);
        printf("z%f\n", getVoltage());

        last = getSystemTime();
    }
}

void motorToggle(void) {
    motorState = !motorState;
    if (motorState) {
        baseSpeed = speed = 10;
        printf("Activated!\n");
    } else {
        baseSpeed = 0;
        printf("Deactivated!\n");
    }
}

void motorUp(void) {
    if (speed <= 250) {
        speed += 5;
        printf("Throttle up to %i\n", speed);
        if (motorState)
            baseSpeed = speed;
    }
}

void motorDown(void) {
    if (speed >= 5) {
        speed -= 5;
        printf("Throttle down to %i\n", speed);
        if (motorState)
            baseSpeed = speed;
    }
}

void motorForward(void) {
    if (targetPitch >= -40) {
        targetPitch -= 5;
        o_should[PITCH] = targetPitch;
        printf("Pitch Forward %i\n", targetPitch);
    }
}

void motorBackward(void) {
    if (targetPitch <= 40) {
        targetPitch += 5;
        o_should[PITCH] = targetPitch;
        printf("Pitch Backwards %i\n", targetPitch);
    }
}

void motorLeft(void) {
    if (targetRoll <= 40) {
        targetRoll += 5;
        o_should[ROLL] = targetRoll;
        printf("Roll Left %i\n", targetRoll);
    }
}

void motorRight(void) {
    if (targetRoll >= -40) {
        targetRoll -= 5;
        o_should[ROLL] = targetRoll;
        printf("Roll Right %i\n", targetRoll);
    }
}

void printRaw(void) {
    Vector v;
    accRead(&v);
    printf("Ax: %f Ay: %f Az: %f\n", v.x, v.y, v.z);
    gyroRead(&v);
    printf("Gx: %f Gy: %f Gz: %f\n", v.x, v.y, v.z);
    magRead(&v);
    printf("Mx: %f My: %f Mz: %f\n\n", v.x, v.y, v.z);
}
