/*
 * test_ram.c
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
#include <util/delay.h>

#include <xycontrol.h>
#include <xmem.h>

#define CHECKSIZE 53248 // 52KB

// Both lights will be red if malloc failed
// One light will be red if the data is corrupt
// Both green lights will show if banks are okay

void check(uint8_t *data, uint8_t mode) {
    for (uint16_t i = 0; i < CHECKSIZE; i++) {
        uint8_t c;
        if (mode) {
            c = 0xFF - (i & 0xFF);
        } else {
            c = (i & 0xFF);
        }
        if (data[i] != c) {
            xyLed(0, 1);
        }
    }
    xyLed(2 + mode, 1);
}

int main(void) {
    xyInit();
    uint8_t bank = 0;

    for(;;) {
        xyLed(4, 0);

        xmemSetBank(bank);
        uint8_t *data = (uint8_t *)malloc(CHECKSIZE);
        xmemSetBank(bank + 1);
        uint8_t *data2 = (uint8_t *)malloc(CHECKSIZE);

        if ((data == NULL) || (data2 == NULL)) {
            xyLed(0, 1);
            xyLed(1, 1);
            while(1);
        }

        for (uint16_t i = 0; i < CHECKSIZE; i++) {
            xmemSetBank(bank);
            data[i] = i & 0xFF;
            xmemSetBank(bank + 1);
            data2[i] = 0xFF - (i & 0xFF);
        }

        xmemSetBank(bank);
        check(data, 0);
        xmemSetBank(bank + 1);
        check(data2, 1);

        xmemSetBank(bank);
        free(data);
        xmemSetBank(bank + 1);
        free(data2);

        _delay_ms(2500);

        if (bank < (MEMBANKS - 2)) {
            bank += 2;
        } else {
            bank = 0;
        }
    }

    return 0;
}
