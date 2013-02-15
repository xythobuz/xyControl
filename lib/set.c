/*
 * set.c
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
#include <control.h>
#include <config.h>

#define SETDELAY (1000 / SET_FREQ)
uint8_t baseSpeed;

void setMotorSpeeds(uint8_t axis, uint8_t *vals) {
    if (axis == ROLL) {
        motorSet(1, vals[0]);
        motorSet(3, vals[1]);
    } else if (axis == PITCH) {
        motorSet(0, vals[0]);
        motorSet(2, vals[1]);
    }
}

void setTask(void) {
    static time_t lastTaskExec = SET_OFFSET;
    if ((getSystemTime() - lastTaskExec) >= SETDELAY) {
        for (uint8_t i = 0; i < 2; i++) {
            double diff = o_output[i];
            if (diff > 0) {
                if (diff > (baseSpeed / 2)) {
                    diff = baseSpeed / 2;
                }
            } else if (diff < 0) {
                if (diff < (-1 * (baseSpeed / 2))) {
                    diff = -1 * (baseSpeed / 2);
                }
            }
            uint8_t v[2];
            if ((baseSpeed + diff) > 255) {
                v[0] = 255;
            } else {
                v[0] = baseSpeed + diff;
            }
            if ((baseSpeed - diff) < 0) {
                v[1] = 0;
            } else {
                v[1] = baseSpeed - diff;
            }
            setMotorSpeeds(i, v);
        }
        lastTaskExec = getSystemTime();
    }
}
