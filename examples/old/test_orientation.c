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
#include <time.h>

// Out of lazyness, we use stdio
int output(char c, FILE *f) {
    serialWrite(c);
    return 0;
}

int input(FILE *f) {
    while (!serialHasChar());
    return serialGet();
}

// You may need to play with these values
#define NORMALIZE 7 // to zero
#define NORMVAL 550 // Gyro reads 550 when 90 DPS are applied.
#define TIMECONST (1.0)
#define FREQ 2 // defines how much serial output you get

#define NORMFACTOR (NORMVAL / 90)
#define DT (1 / FREQ) // s
#define DELAY (1000 / FREQ) // ms
#define TODEG(x) ((x * 180) / M_PI)

void normalize(Vector *v) {
    v->x = ((v->x + NORMALIZE) / NORMFACTOR);
    v->y = ((v->y + NORMALIZE) / NORMFACTOR);
    v->z = ((v->z + NORMALIZE) / NORMFACTOR);
}

/*
 * The real magic is happening here and,
 * to be honest, I don't really understand it.
 * But, hey, at least it works...
 */
double complementary(double angle, double rate, double last) {
    return ((((angle - last) * square(TIMECONST) * DT) + ((angle - last) * 2 * TIMECONST) + rate) * DT) + angle;
}

int main(void) {
    xyInit();
    xyLed(4, 0);

    fdevopen(&output, NULL); // stdout & stderr
    fdevopen(NULL, &input); // stdin

    printf("Orientation Test\n");
    _delay_ms(100); // Wait for Sensors
    gyroInit(r250DPS);
    printf("Gyroscope initialized!\n");
    accInit(r2G);
    printf("Accelerometer initialized!\n");

    double filteredAngle = 0;
    double filteredPitch = 0;

    for(;;) {
        time_t start = getSystemTime();

        xyLed(2, 2);
        xyLed(3, 2); // Toggle Green LEDs

        Vector g, a;
        accRead(&a); // Read Accelerometer
        gyroRead(&g); // Read Gyroscope
        normalize(&g); // Normalize Gyroscope Data

        // Calculate Pitch & Roll from Accelerometer Data
        double roll = atan((double)a.y / hypot((double)a.x, (double)a.z));
        double pitch = atan((double)a.x / hypot((double)a.y, (double)a.z));
        roll = TODEG(roll);
        pitch = TODEG(pitch); // As Degree, not radians!

        // Filter Roll and Pitch with Gyroscope Data from the corresponding axis
        filteredAngle = complementary(roll, g.x, filteredAngle);
        filteredPitch = complementary(pitch, g.y, filteredPitch);

        printf("Roll:  %f\n", filteredAngle);
        printf("Pitch: %f\n", filteredPitch);
        printf("\n");

        while ((getSystemTime() - start) < DELAY);
    }

    return 0;
}
