/*
 * control.c
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
#include <pid.h>
#include <orientation.h>
#include <control.h>
#include <config.h>

#define CONTROLDELAY (1000 / CON_FREQ)

#define ROLL 0
#define PITCH 1

PIDState o_pids[2];
double o_should[2];
double o_output[2];

void controlInit(void) {
    for (uint8_t i = 0; i < 2; i++) {
        o_pids[i].kp = CON_P;
        o_pids[i].ki = CON_I;
        o_pids[i].kd = CON_D;
        o_pids[i].lastError = 0;
        o_pids[i].sumError = 0;
        o_pids[i].last = 0;
        o_should[i] = 0.0;
    }
    addTask(&controlTask);
}

void controlTask(void) {
    static time_t lastTaskExec = CON_OFFSET;
    if ((getSystemTime() - lastTaskExec) >= CONTROLDELAY) {
        for (uint8_t i = 0; i < 2; i++) {
            double is;
            if (i == ROLL) {
                is = orientation.roll;
            } else if (i == PITCH) {
                is = orientation.pitch;
            }
            o_output[i] = pidExecute(o_should[i], is, &o_pids[i]);
        }
        lastTaskExec = getSystemTime();
    }
}
