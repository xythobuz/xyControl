/*
 * motor.c
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
#include <avr/io.h>

#include <twi.h>
#include <motor.h>
#include <tasks.h>
#include <time.h>
#include <config.h>

#define MOTORCOUNT 4
#define MOTORDELAY 10 // Update every MOTORDELAY milliseconds

uint8_t motorSpeed[MOTORCOUNT];

void motorTask(void) {
    static time_t lastTaskExec = 0;
    if ((getSystemTime() - lastTaskExec) >= MOTORDELAY) {
        for (uint8_t i = 0; i < MOTORCOUNT; i++) {
            twiStart(MOTOR_BASEADDRESS + (i << 1) + TWI_WRITE);
            twiWrite(motorSpeed[i]);
            twiStop();
        }
        lastTaskExec = getSystemTime();
    }
}

void motorInit(void) {
    for (uint8_t i = 0; i < MOTORCOUNT; i++) {
        motorSpeed[i] = 0;
    }
    addTask(&motorTask);
}

void motorSet(uint8_t id, uint8_t speed) {
    if (id < MOTORCOUNT) {
        motorSpeed[id] = speed;
    } else {
        for (id = 0; id < MOTORCOUNT; id++) {
            motorSpeed[id] = speed;
        }
    }
}
