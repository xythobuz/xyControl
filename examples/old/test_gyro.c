/*
 * test_gyro.c
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

// Out of lazyness, we use stdio
int output(char c, FILE *f) {
    serialWrite(c);
    return 0;
}

int input(FILE *f) {
    while (!serialHasChar());
    return serialGet();
}

// Runge-Kutta Integration
double integrate(double last, double v0, double v1, double v2, double v3) {
    double result = last + ((v3 + (2 * v2) + (2 * v1) + v0) / 6);
    return result;
}

// Take a look at this: http://tom.pycke.be/mav/70/
// First, we normalize the data (that's the "+ 7").
// Then, we integrate it using the Runge-Kutta Integrator.
// Last, we scale it ("/ 15").
// Notice the very large drift without accelerometer compensation!

int main(void) {
    xyInit();
    Vector v[4];
    xyLed(4, 0);

    fdevopen(&output, NULL); // stdout & stderr
    fdevopen(NULL, &input); // stdin

    gyroInit(r250DPS);
    gyroRead(&v[2]);
    gyroRead(&v[1]);
    gyroRead(&v[0]);

    double xResult = 0;

    for(;;) {
        xyLed(2, 2);
        xyLed(3, 2); // Toggle Green LEDs

        v[3] = v[2];
        v[2] = v[1];
        v[1] = v[0];
        gyroRead(&v[0]);

        // Integrate on X-Axis
        xResult = integrate(xResult, v[0].x + 7, v[1].x + 7, v[2].x + 7, v[3].x + 7);

        printf("Data:  %f\n", (double)(v[0].x + 7));
        printf("Angle: %f\n", xResult / 15);

        _delay_ms(500);
    }

    return 0;
}
