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
#include <util/delay.h>

#include <xycontrol.h>
#include <config.h>
#include <servos.h>

#define STATE_GOHIGH 0
#define STATE_GOLOW 1

typedef struct {
    uint8_t id;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t pin;
    uint16_t width;
    uint16_t next;
} Servo;

extern Servo servos[4];

Servo servos[4] = {
    {.port = &SERVO0PORT, .ddr = &SERVO0DDR, .pin = SERVO0PIN},
    {.port = &SERVO1PORT, .ddr = &SERVO1DDR, .pin = SERVO1PIN},
    {.port = &SERVO2PORT, .ddr = &SERVO2DDR, .pin = SERVO2PIN},
    {.port = &SERVO3PORT, .ddr = &SERVO3DDR, .pin = SERVO3PIN},
};

inline void sortServos(void) {
    Servo tmp;
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = i + 1; j < 4; j++) {
            if (servos[i].width > servos[j].width) {
                tmp = servos[i];
                servos[i] = servos[j];
                servos[j] = tmp;
            }
        }
    }
}

ISR(TIMER1_COMPA_vect) {
    static uint8_t state = STATE_GOHIGH;
    static uint8_t servoCount;

    if (state == STATE_GOHIGH) {
        for (uint8_t i = 0; i < 4; i++) {
            *(servos[i].port) |= (1 << servos[i].pin);
            servos[i].width = servos[i].next;
        }
        sortServos();
        servoCount = 0;
        OCR1A = servos[servoCount].width;
        state = STATE_GOLOW;
    } else if (state == STATE_GOLOW) {
        *(servos[servoCount].port) &= ~(1 << servos[servoCount].pin);
        if (servoCount == 3) {
            // was last servo
            OCR1A = WIDTH - servos[servoCount].width;
            state = STATE_GOLOW;
        } else {
            uint16_t sum = 0;
            servoCount++;
            for (uint8_t i = 0; i < servoCount; i++) {
                sum += servos[i].width;
            }
            OCR1A = servos[servoCount].width - sum;
        }
    }
}

void servosInit(void) {
    // Set Servo Pins to output
    for (uint8_t i = 0; i < 4; i++) {
        *(servos[i].ddr) |= (1 << servos[i].pin);
        servos[i].width = INIT;
        servos[i].next = INIT;
        servos[i].id = i;
    }

    TCCR1B |= (1 << WGM12) | (1 << CS11); // Timer1 CTC Mode Prescaler 8
    OCR1A = 100;
    TIMSK1 |= (1 << OCIE1A); // Overflow Interrupt
    sei();

    // Init sequence
    _delay_ms(5000);
    for (uint8_t i = 0; i < 4; i++) {
        servos[i].next = MIN;
    }
    _delay_ms(2000);
}

void servoPos(uint8_t servo, uint16_t value) {
    if (servo < 4) {
        for (uint8_t i = 0; i < 4; i++) {
            if (servos[i].id == servo) {
                servos[i].next = value;
                return;
            }
        }
    } else {
        for (uint8_t i = 0; i < 4; i++) {
            servoPos(i, value);
        }
    }
}
