/*
 * adc.c
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

#include <adc.h>

/** \addtogroup adc ADC Driver
 *  \ingroup Hardware
 *  @{
 */

/** \file adc.c
 *  Analog-to-Digital Converter API Implementation
 */

void adcInit(ADCRef ref) {
    // Enable ADC Module, start one conversion, wait for finish
    PRR0 &= ~(1 << PRADC); // Disable ADC Power Reduction (Enable it...)
    switch(ref) {
        case AVCC:
            ADMUX = (1 << REFS0);
            break;

        case AINT1:
            ADMUX = (1 << REFS1);
            break;

        case AINT2:
            ADMUX = (1 << REFS1) | (1 << REFS0);
            break;

        case AREF:
            ADMUX &= ~((1 << REFS0) | (1 << REFS1));
            break;
    }

    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
    ADCSRB = 0;
    ADCSRA |= (1 << ADEN) | (1 << ADSC); // Start ADC, single conversion
}

void adcStart(uint8_t channel) {
    // Start a measurement on channel
    if (channel > 15) {
        channel = 0;
    }
    if (channel > 7) {
        channel -= 8;
        ADCSRB |= (1 << MUX5);
    } else {
        ADCSRB &= ~(1 << MUX5);
    }
    ADMUX &= ~0x1F; // Delete MUX0:4
    ADMUX |= channel;
    ADCSRA |= (1 << ADSC);
}

uint8_t adcReady() {
    // Is the measurement finished
    if (ADCSRA & (1 << ADSC)) {
        // ADSC bit is set
        return 0;
    } else {
        return 1;
    }
}

uint16_t adcGet(uint8_t next) {
    // Return measurements result
    // Start next conversion
    uint16_t temp = 0;
    while (!adcReady());
    temp = ADC;
    if (next)
        ADCSRA |= (1 << ADSC); // Start next conversion
    return temp;
}

void adcClose() {
    // deactivate adc
    ADCSRA &= ~(1 << ADSC);
    PRR0 |= (1 << PRADC);
    ADCSRA &= ~(1 << ADEN);
}
/** @} */
