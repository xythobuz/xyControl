/*
 * test_servo.c
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
#include <avr/interrupt.h>
#include <util/delay.h>

#include <xycontrol.h>
#include <xmem.h>

#define SERVOPORT PORTJ
#define SERVODDR DDRJ
#define SERVOPIN PJ4

// Prescaler: 8 --> 2MHz
#define MIN 2000 // 2000 ------> 1ms
#define MAX 4000 // 4000 ------> 2ms
#define WIDTH 40000 // 40000 --> 20ms

#define MULT 30
#define CONSTANT ((MAX - MIN) * MULT / 255)

volatile uint8_t currentPin = 0;
volatile uint16_t next;

ISR(TIMER1_COMPA_vect) {
    if (OCR1A != next) {
        if (currentPin == 0) {
            OCR1A = next;
        } else {
            OCR1A = WIDTH - OCR1A;
        }
    } else {
        OCR1A = WIDTH - OCR1A;
        next = OCR1A;
    }

    if (currentPin) {
        currentPin = 0;
        SERVOPORT &= ~(1 << SERVOPIN);
    } else {
        currentPin = 1;
        SERVOPORT |= (1 << SERVOPIN);
    }
}

void ledShowNumber(uint8_t i) {
    xyLed(4, 0);
    if (i & 0x01) {
        xyLed(0, 1);
    }
    if (i & 0x02) {
        xyLed(1, 1);
    }
    if (i & 0x04) {
        xyLed(2, 1);
    }
    if (i & 0x08) {
        xyLed(3, 1);
    }
}

int main(void) {
    xyInit();
    SERVODDR |= (1 << SERVOPIN);

    TCCR1B |= (1 << WGM12) | (1 << CS11); // Timer1 CTC Mode Prescaler 8
    OCR1A = MIN;
    next = MIN;
    TIMSK1 |= (1 << OCIE1A); // Overflow Interrupt
    sei();

    _delay_ms(6000);

    for (;;) {
        for (uint8_t i = 0; i < 16; i++) {
            next = MIN + (125 * (i + 1));
            ledShowNumber(i);
            _delay_ms(1000);
        }
        _delay_ms(2000);
        for (uint8_t i = 16; i > 0; i--) {
            next = MIN + (125 * (i - 1));
            ledShowNumber(i);
            _delay_ms(1000);
        }
        _delay_ms(2000);
    }

    return 0;
}
