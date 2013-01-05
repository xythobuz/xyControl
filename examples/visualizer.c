/*
 * visualizer.c
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

#include <xycontrol.h>
#include <serial.h>
#include <acc.h>

int main(void) {
    xyInit();
    xyLed(4, 0);
    xyLed(0, 1);
    xyLed(2, 1);

    accInit(r2G);

    for(;;) {
        xyLed(2, 2);
        xyLed(3, 2); // Toggle Green LEDs

        while (!serialHasChar());
        char c = serialGet();
        Vector v;
        if (c == 'a') {
            accRead(&v);
        } else {
            xyLed(0, 2);
            xyLed(1, 2); // Toggle Red LEDs
        }

        serialWrite(((*((uint16_t *)(&v.a))) & 0xFF00) >> 8);
        serialWrite((*((uint16_t *)(&v.a))) & 0xFF);
        serialWrite(((*((uint16_t *)(&v.b))) & 0xFF00) >> 8);
        serialWrite((*((uint16_t *)(&v.b))) & 0xFF);
        serialWrite(((*((uint16_t *)(&v.c))) & 0xFF00) >> 8);
        serialWrite((*((uint16_t *)(&v.c))) & 0xFF);
    }

    return 0;
}
