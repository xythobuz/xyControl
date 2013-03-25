/*
 * pid.c
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
#include <config.h>

/** \addtogroup pid PID-Controller
 *  \ingroup Flight
 *  @{
 */

/** \file pid.c
 *  PID Library Implementation
 */

PIDState o_pids[2];
double o_should[2];
double o_output[2];

double pidExecute(double should, double is, PIDState *state) {
    time_t now = getSystemTime();
    double timeChange = (double)(now - state->last);
    double error = should - is;
    double newErrorSum = state->sumError + (error * timeChange);
    if ((newErrorSum >= state->intMin) && (newErrorSum <= state->intMax))
        state->sumError = newErrorSum; // Prevent Integral Windup
    double dError = (error - state->lastError) / timeChange;
    double output = (state->kp * error) + (state->ki * state->sumError) + (state->kd * dError);
    state->lastError = error;
    state->last = now;
    if (output > state->outMax) {
        output = state->outMax;
    }
    if (output < state->outMin) {
        output = state->outMin;
    }
    return output;
}

void pidInit(void) {
    for (uint8_t i = 0; i < 2; i++) {
        pidSet(&o_pids[i], PID_P, PID_I, PID_D, PID_OUTMIN, PID_OUTMAX, PID_INTMIN, PID_INTMAX);
        o_should[i] = 0.0;
    }
}

void pidSet(PIDState *pid, double kp, double ki, double kd, double min, double max, double iMin, double iMax) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->outMin = min;
    pid->outMax = max;
    pid->intMin = iMin;
    pid->intMax = iMax;
    pid->lastError = 0;
    pid->sumError = 0;
    pid->last = 0;
}

void pidTask(void) {
    o_output[ROLL] = pidExecute(o_should[ROLL], orientation.roll, &o_pids[ROLL]);
    o_output[PITCH] = pidExecute(o_should[PITCH], orientation.pitch, &o_pids[PITCH]);
}
/** @} */
