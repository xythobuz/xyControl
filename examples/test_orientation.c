/*
 * test_orientation.c
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
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#include <xycontrol.h>
#include <serial.h>
#include <gyro.h>
#include <acc.h>

// Out of lazyness, we use stdio
int output(char c, FILE *f) {
    serialWrite(c);
    return 0;
}

int input(FILE *f) {
    while (!serialHasChar());
    return serialGet();
}

// Take a look at this: http://tom.pycke.be/mav/70/
// First, we normalize the data.
// Then, we integrate it using the Runge-Kutta Integrator.
// Last, we scale it.
// Notice the very large drift without accelerometer compensation!
#define NORMALIZE 7 // to zero
#define NORMVAL 550 // Gyro reads 550...
#define NORMDPS 90 // ...when 90 DPS are applied.
#define NORMFACTOR (NORMVAL / NORMDPS)

void normalize(Vector *v) {
    v->x = ((v->x + NORMALIZE) / NORMFACTOR);
    v->y = ((v->y + NORMALIZE) / NORMFACTOR);
    v->z = ((v->z + NORMALIZE) / NORMFACTOR);
}

double complementary(double angle, double rate, double last) {
    double timeConstant = 1.0;
    double dt = 0.2;
    return ((((angle - last) * square(timeConstant) * dt) + ((angle - last) * 2 * timeConstant) + rate) * dt) + angle;
}

int main(void) {
    xyInit();
    Vector v;
    xyLed(4, 0);

    fdevopen(&output, NULL); // stdout & stderr
    fdevopen(NULL, &input); // stdin

    gyroInit(r250DPS);
    accInit(r2G);

    double filteredAngle = 0;
    double filteredPitch = 0;

    for(;;) {
        xyLed(2, 2);
        xyLed(3, 2); // Toggle Green LEDs

        gyroRead(&v);
        normalize(&v);

        Vector a;
        accRead(&a);
        double roll = atan((double)a.y / hypot((double)a.x, (double)a.z));
        double pitch = atan((double)a.x / hypot((double)a.y, (double)a.z));
        roll = (roll * 180) / M_PI;
        pitch = (pitch * 180) / M_PI;
        filteredAngle = complementary(roll, v.x, filteredAngle);
        filteredPitch = complementary(pitch, v.y, filteredPitch);

        printf("Roll:  %f\n", filteredAngle);
        printf("Pitch: %f\n", filteredPitch);
        printf("\n");

        _delay_ms(200);
    }

    return 0;
}
