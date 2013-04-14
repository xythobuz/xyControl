/*
 * complementary.c
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
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

#include <time.h>
#include <complementary.h>
#include <config.h>

/** \addtogroup complementary Complementary-Filter
 *  \ingroup Flight
 *  @{
 */

/** \file complementary.c
 *  Complementary-Filter Implementation.
 */

void complementaryInit(Complementary *data) {
    data->angle = 0;
    data->lastExecute = getSystemTime();
}

void complementaryExecute(Complementary *data, double acc, double gyro) {
    time_t dtInteger = (getSystemTime() - data->lastExecute);
    double dt = dtInteger / 1000;
    data->angle = (data->angle + (gyro * dt)); // Gyro Integrator
    data->angle *= COMPLEMENTARY_TAU / (COMPLEMENTARY_TAU + dt); // High-Pass
    data->angle += (1 - (COMPLEMENTARY_TAU / (COMPLEMENTARY_TAU + dt))) * acc; // Low-Pass
    data->lastExecute = getSystemTime();
}
/** @} */
