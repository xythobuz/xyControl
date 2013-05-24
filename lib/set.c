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
#include <pid.h>
#include <set.h>
#include <config.h>

/** \addtogroup set Motor Speed Mixer
 *  \ingroup Flight
 *  Takes the Base Speed and PID-Output and sets Motor Speed accordingly.
 *  @{
 */

/** \file set.c
 *  Motor Mixer Library Implementation
 */

uint8_t baseSpeed = 0; /**< Motor Base Speed */

/** Set the Motor Speeds according to the SET_* Motor Position Constants.
 *  \param axis ROLL or PITCH
 *  \param vals Speeds for the two Motors on this axis (+, -)
 */
inline void setMotorSpeeds(uint8_t axis, uint8_t *vals) {
    if (axis == ROLL) {
        motorSet(SET_ROLLPLUS, vals[0]);
        motorSet(SET_ROLLMINUS, vals[1]);
    } else if (axis == PITCH) {
        motorSet(SET_PITCHPLUS, vals[0]);
        motorSet(SET_PITCHMINUS, vals[1]);
    }
}

void setTask(void) {
    for (uint8_t i = 0; i < 2; i++) {
        uint8_t diff = pidOutput[i];
        if (diff > baseSpeed) {
            diff = baseSpeed; // Limit PID
        }
        uint8_t v[2] = { baseSpeed + diff, baseSpeed - diff };
        setMotorSpeeds(i, v);
    }
}
/** @} */
