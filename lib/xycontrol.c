/*
 * xycontrol.c
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
#include <avr/interrupt.h>

#include <serial.h>
#include <spi.h>
#include <time.h>
#include <xmem.h>
#include <xycontrol.h>
#include <twi.h>
#include <adc.h>
#include <config.h>

void xyInit(void) {
    xmemInit(); // Most important!

    // LEDs
    LED0DDR |= (1 << LED0PIN);
    LED1DDR |= (1 << LED1PIN);
    LED2DDR |= (1 << LED2PIN);
    LED3DDR |= (1 << LED3PIN);
    xyLed(4, 1);

    initSystemTimer();
    serialInit(BAUD(38400, F_CPU));
    twiInit();
    adcInit(AVCC);

    sei();
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
        xyLedInternal(v, &LED0PORT, LED0PIN);
    } else if (l == 1) {
        xyLedInternal(v, &LED1PORT, LED1PIN);
    } else if (l == 2) {
        xyLedInternal(v, &LED2PORT, LED2PIN);
    } else if (l == 3) {
        xyLedInternal(v, &LED3PORT, LED3PIN);
    } else {
        xyLed(0, v);
        xyLed(1, v);
        xyLed(2, v);
        xyLed(3, v);
    }
}

double getVoltage(void) {
    adcStart(BATT_CHANNEL);
    while(!adcReady());
    uint16_t v = adcGet(0) * BATT_MAX;
    return ((double)v / 1024.0);
}
