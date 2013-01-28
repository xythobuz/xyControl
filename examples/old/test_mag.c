/*
 * test_mag.c
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
#include <math.h>
#include <util/delay.h>

#include <xycontrol.h>
#include <serial.h>
#include <mag.h>

// Out of lazyness, we use stdio
int output(char c, FILE *f) {
    serialWrite(c);
    return 0;
}

int input(FILE *f) {
    while (!serialHasChar());
    return serialGet();
}

int main(void) {
    xyInit();
    xyLed(4, 0);

    fdevopen(&output, NULL); // stdout & stderr
    fdevopen(NULL, &input); // stdin

    magInit(r1g9);

    for(;;) {
        xyLed(2, 2);
        xyLed(3, 2); // Toggle Green LEDs
        Vector v;
        magRead(&v);

        double x = (double)v.x;
        double y = (double)v.y;
        double z = (double)v.z;

        printf("X: %f\nY: %f\nZ: %f\n", x, y, z);

        _delay_ms(500);
    }

    return 0;
}
