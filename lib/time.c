/*
 * time.c
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
#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include <time.h>

/** \addtogroup time Time Keeping
 *  \ingroup System
 *  @{
 */

/** \file time.c
 *  Time API Implementation.
 */

volatile time_t systemTime = 0; /**< Current System Uptime */

#define TCRA TCCR2A /**< Timer 2 Control Register A */
#define TCRB TCCR2B /**< Timer 2 Control Register B */
#define OCR OCR2A /**< Timer 2 Compare Register A */
#define TIMS TIMSK2 /**< Timer 2 Interrupt Mask */
#define OCIE OCIE2A /**< Timer 2 Compare Match A Interrupt Enable */

void initSystemTimer() {
    // Timer initialization
    TCRA |= (1 << WGM21); // CTC Mode
    TCRB |= (1 << CS22); // Prescaler: 64
    OCR = 250;
    TIMS |= (1 << OCIE); // Enable compare match interrupt
}

/** Timer 2 Compare Match A Interrupt */
ISR(TIMER2_COMPA_vect) {
    systemTime++;
}

time_t getSystemTime(void) {
    return systemTime;
}
/** @} */
