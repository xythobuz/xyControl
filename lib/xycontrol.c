/*
 * xycontrol.c
 *
 * Copyright (c) 2012, Thomas Buck <xythobuz@me.com>
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

#include <serial.h>
#include <spi.h>
#include <time.h>
#include <xmem.h>

void xyInit(void) {
    // LEDs
    DDRL |= (1 << PL7) | (1 << PL6);
    DDRG |= (1 << PG5);
    DDRE |= (1 << PE2);
    PORTL |= (1 << PL6) | (1 << PL7);
    PORTG |= (1 << PG5);
    PORTE |= (1 << PE2);

    initSystemTimer();
    serialInit(BAUD(38400, F_CPU));
    spiInit();
    xmemInit();
}

void xyLedInternal(uint8_t v, volatile uint8_t *port, uint8_t pin) {
    if (v == 0) {
        *port &= ~(1 << pin);
    } else if (v == 1) {
        *port |= (1 << pin);
    } else {
        *port ^= (1 << pin);
    }
}

void xyLed(uint8_t l, uint8_t v) {
    if (l == 0) {
        xyLedInternal(v, &PORTL, PL6);
    } else if (l == 1) {
        xyLedInternal(v, &PORTL, PL7);
    } else if (l == 2) {
        xyLedInternal(v, &PORTG, PL5);
    } else if (l == 3) {
        xyLedInternal(v, &PORTE, PL2);
    } else {
        xyLed(0, v);
        xyLed(1, v);
        xyLed(2, v);
        xyLed(3, v);
    }
}
