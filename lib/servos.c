/*
 * servos.c
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

#include <xycontrol.h>
#include <config.h>

// Prescaler: 8 --> 2MHz
#define MIN 2000 // 2000 ------> 1ms
#define MAX 4000 // 4000 ------> 2ms
#define WIDTH 40000 // 40000 --> 20ms

typedef struct {
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t pin;
} Servo;

const Servo servos[4] = {
    {.port = &SERVO0PORT, .ddr = &SERVO0DDR, .pin = SERVO0PIN},
    {.port = &SERVO1PORT, .ddr = &SERVO1DDR, .pin = SERVO1PIN},
    {.port = &SERVO2PORT, .ddr = &SERVO2DDR, .pin = SERVO2PIN},
    {.port = &SERVO3PORT, .ddr = &SERVO3DDR, .pin = SERVO3PIN},
};

void servosInit(void) {
    // Set Servo Pins to output
    for (uint8_t i = 0; i < 4; i++) {
        *(servos[i].ddr) = (1 << servos[i].pin);
    }

    TCCR1B |= (1 << WGM12) | (1 << CS11); // Timer1 CTC Mode Prescaler 8
    OCR1A = MIN - 125;
    // next = MIN - 125;
    TIMSK1 |= (1 << OCIE1A); // Overflow Interrupt
    sei();

    // _delay_ms(5000);
    // next = MIN;
    // _delay_ms(2000);
}

ISR(TIMER1_COMPA_vect) {

}
