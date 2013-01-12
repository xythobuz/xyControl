/*
 * test_motorlevel.c
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
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include <xycontrol.h>
#include <motor.h>
#include <tasks.h>
#include <time.h>
#include <acc.h>
#include <gyro.h>
#include <serial.h>

#define CONTROLDELAY 50

// Motor Positions
#define ROLLPLUS 0
#define ROLLMINUS 2
#define PITCHPLUS 3
#define PITCHMINUS 1

// You may need to play with these values
#define NORMALIZE 7 // to zero
#define NORMVAL 550 // Gyro reads 550 when 90 DPS are applied.
#define TIMECONST (1.0)
#define FREQ 2 // defines how much serial output you get

#define NORMFACTOR (NORMVAL / 90)
#define DT (1 / FREQ) // s
#define DELAY (1000 / FREQ) // ms
#define TODEG(x) ((x * 180) / M_PI)

double lastRoll = 0;
double lastPitch = 0;
double targetRoll = 0;
double targetPitch = 0;
uint8_t baseSpeed = 10;
int8_t modRoll = 0;
int8_t modPitch = 0;

void normalize(Vector *v) {
    v->x = ((v->x + NORMALIZE) / NORMFACTOR);
    v->y = ((v->y + NORMALIZE) / NORMFACTOR);
    v->z = ((v->z + NORMALIZE) / NORMFACTOR);
}

double complementary(double angle, double rate, double last) {
    return ((((angle - last) * square(TIMECONST) * DT) + ((angle - last) * 2 * TIMECONST) + rate) * DT) + angle;
}

void calculateAngles(void) {
    Vector g, a;
    accRead(&a); // Read Accelerometer
    gyroRead(&g); // Read Gyroscope
    normalize(&g); // Normalize Gyroscope Data
    double roll = atan((double)a.y / hypot((double)a.x, (double)a.z));
    double pitch = atan((double)a.x / hypot((double)a.y, (double)a.z));
    roll = TODEG(roll);
    pitch = TODEG(pitch);
    lastRoll = complementary(roll, g.x, lastRoll);
    lastPitch = complementary(pitch, g.y, lastPitch);
}

void setMotorSpeeds(void) {
    int16_t motorSpeed;
    for (uint8_t i = 0; i < 4; i++) {
        motorSpeed = baseSpeed;
        if (i == ROLLPLUS) {
            motorSpeed += modRoll;
        } else if (i == ROLLMINUS) {
            motorSpeed -= modRoll;
        } else if (i == PITCHPLUS) {
            motorSpeed += modPitch;
        } else if (i == PITCHMINUS) {
            motorSpeed -= modPitch;
        }
        if (motorSpeed > 255) {
            motorSet(i, 255);
        } else if (motorSpeed < 0) {
            motorSet(i, 0);
        } else {
            motorSet(i, (uint8_t)motorSpeed);
        }
    }
}

void controlTask(void) {
    static time_t lastExec = 0;
    if ((getSystemTime() - lastExec) >= CONTROLDELAY) {
        calculateAngles();
        if (lastRoll > targetRoll) {
            if (modRoll < INT8_MAX)
                modRoll++;
        } else if (lastRoll < targetRoll) {
            if (modRoll > INT8_MIN)
                modRoll--;
        }
        if (lastPitch > targetPitch) {
            if (modPitch < INT8_MAX)
                modPitch++;
        } else if (lastPitch < targetPitch) {
            if (modPitch > INT8_MIN)
                modPitch--;
        }
        setMotorSpeeds();
    }
}

void serialTask(void) {
    if (serialHasChar()) {
        char c = serialGet();
        switch (c) {
            case 'h':
                serialWriteString("+-\n");
                break;
            case '+':
                if (baseSpeed <= 245)
                    baseSpeed += 10;
                break;
            case '-':
                if (baseSpeed >= 10)
                    baseSpeed -= 10;
                break;
        }
    }
}

int main(void) {
    xyInit();
    xyLed(4, 0);
    xyLed(2, 1);
    xyLed(3, 1);

    gyroInit(r250DPS);
    accInit(r2G);
    motorInit();

    addTask(&controlTask);
    addTask(&serialTask);

    for(;;) {
        tasks();
    }

    return 0;
}
