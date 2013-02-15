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
#include <control.h>
#include <set.h>

void motorToggle(void);
void motorUp(void);
void motorDown(void);
void motorForward(void);
void motorBackward(void);
void motorLeft(void);
void motorRight(void);
void printOrientation(void);
void printVoltage(void);
void batteryWarner(void);

char motorToggleString[] PROGMEM = "Motor On/Off";
char motorUpString[] PROGMEM = "Up";
char motorDownString[] PROGMEM = "Down";
char motorLeftString[] PROGMEM = "Left";
char motorRightString[] PROGMEM = "Right";
char motorForwardString[] PROGMEM = "Forwards";
char motorBackwardString[] PROGMEM = "Backwards";
char orientationString[] PROGMEM = "Print Orientation";
char voltageString[] PROGMEM = "Battery Voltage";

uint8_t motorState = 0;
uint8_t speed = 10;
int16_t targetRoll = 0;
int16_t targetPitch = 0;

int main(void) {
    xyInit();
    xyLed(LED_ALL, LED_ON);
    accInit(r2G);
    gyroInit(r250DPS);
    magInit(r1g9);

    addTask(&orientationTask);
    addTask(&controlTask);
    addTask(&setTask);
    motorInit();
    addTask(&batteryWarner);

    addMenuCommand('o', orientationString, &printOrientation);
    addMenuCommand('v', voltageString, &printVoltage);
    addMenuCommand('m', motorToggleString, &motorToggle);
    addMenuCommand('w', motorForwardString, &motorForward);
    addMenuCommand('a', motorLeftString, &motorLeft);
    addMenuCommand('s', motorBackwardString, &motorBackward);
    addMenuCommand('d', motorRightString, &motorRight);
    addMenuCommand('+', motorUpString, &motorUp);
    addMenuCommand('-', motorDownString, &motorDown);

    for(;;) {
        tasks();
    }

    return 0;
}

void motorToggle(void) {
    motorState = !motorState;
    if (motorState) {
        baseSpeed = speed;
        printf("On!\n");
    } else {
        baseSpeed = 0;
        printf("Off!\n");
    }
}

void motorUp(void) {
    if (speed <= 250) {
        speed += 5;
        baseSpeed = speed;
        printf("%i\n", speed);
    }
}

void motorDown(void) {
    if (speed >= 5) {
        speed -= 5;
        baseSpeed = speed;
        printf("%i\n", speed);
    }
}

void motorForward(void) {
    if (targetPitch >= -40) {
        targetPitch -= 5;
        o_should[PITCH] = targetPitch;
        printf("%i\n", targetPitch);
    }
}

void motorBackward(void) {
    if (targetPitch <= 40) {
        targetPitch += 5;
        o_should[PITCH] = targetPitch;
        printf("%i\n", targetPitch);
    }
}

void motorLeft(void) {
    if (targetRoll <= 40) {
        targetRoll += 5;
        o_should[ROLL] = targetRoll;
        printf("%i\n", targetRoll);
    }
}

void motorRight(void) {
    if (targetRoll >= -40) {
        targetRoll -= 5;
        o_should[ROLL] = targetRoll;
        printf("%i\n", targetRoll);
    }
}

void printVoltage(void) {
    printf("Battery: %fV\n", getVoltage());
}

void printOrientation(void) {
    printf("Pitch: %f\nRoll: %f\n", orientation.pitch, orientation.roll);
}

void batteryWarner(void) {
    static time_t last = 0;
    if ((getSystemTime() - last) >= 5000) {
        double v = getVoltage();
        if (v < 9.99) {
            if (v > 9.0) {
                printf("Battery empty: %fV\n", v);
            } else {
                printf("!!WARNING!!\nBATTERY EMPTY: %fV\n!!CHARGE NOW!!\n", v);
            }
        }
        last = getSystemTime();
    }
}
