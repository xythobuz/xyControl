/*
 * orientation.c
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
#include <math.h>

#include <xycontrol.h>
#include <gyro.h>
#include <acc.h>
#include <time.h>
#include <orientation.h>
#include <config.h>

#define NORMFACTOR (NORMVAL / NORMVALSPEED)
#define DT (1 / FREQ) // s
#define DELAY (1000 / FREQ) // ms
#define TODEG(x) ((x * 180) / M_PI)

Angles orientation = {.pitch = 0, .roll = 0};

void normalize(Vector *v) {
    v->x = ((v->x + NORMALIZE) / NORMFACTOR);
    v->y = ((v->y + NORMALIZE) / NORMFACTOR);
    v->z = ((v->z + NORMALIZE) / NORMFACTOR);
}

double complementary(double angle, double rate, double last) {
    return ((((angle - last) * square(TIMECONST) * DT) + ((angle - last) * 2 * TIMECONST) + rate) * DT) + angle;
}

void orientationTask(void) {
    static time_t last = 0;
    if ((getSystemTime() - last) >= DELAY) {

        Vector g, a;
        accRead(&a); // Read Accelerometer
        gyroRead(&g); // Read Gyroscope
        normalize(&g); // Normalize Gyroscope Data

        // Calculate Pitch & Roll from Accelerometer Data
        double roll = atan((double)a.x / hypot((double)a.y, (double)a.z));
        double pitch = atan((double)a.y / hypot((double)a.x, (double)a.z));
        roll = TODEG(roll);
        pitch = TODEG(pitch); // As Degree, not radians!

        // Filter Roll and Pitch with Gyroscope Data from the corresponding axis
        orientation.roll = complementary(roll, g.y, orientation.roll);
        orientation.pitch = complementary(pitch, g.x, orientation.pitch);
    }
}
