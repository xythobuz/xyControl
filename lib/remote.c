/*
 * remote.c
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
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <remote.h>

/** \addtogroup remote Remote Control Interface
 *  \ingroup System
 *  @{
 */

/** \file remote.c
 *  Remote API Implementation. Uses Timer0!
 *  Base on http://www.rn-wissen.de/index.php/RC-Empfänger_auswerten#C-Programmbeispiel_.28Auslesen_mit_Timer0.29
 */

#define CONCATx(a,b) a##b
#define CONCAT(a,b) CONCATx(a,b)

#define ISCn0 CONCAT(ISC,CONCAT(RC_EXTINT,0))
#define ISCn1 CONCAT(ISC,CONCAT(RC_EXTINT,1))
#define INTn CONCAT(INT,RC_EXTINT)
#define INTn_vect CONCAT(INT,CONCAT(RC_EXTINT,_vect))

volatile int8_t rcValues[RC_CHANNELS];
volatile uint8_t rcSignalCounter = 0, rcSignalValid = 0;

// 1000 / (16000000 / 256 / 230) = 3,68ms
//#define RC_TIMER_RELOAD 26 // 256 - 230 = 26
#define RC_TIMER_RELOAD 0

/** External Interrupt detecting Timer0 Overflow, invalidating signal matching */
ISR(TIMER0_OVF_vect) {
    rcSignalCounter = 0; // Reset Channel Counter
    rcSignalValid = 0; // Block measurement until next pulse starts
}

/** External Interrupt Service Routine for received sum signal */
ISR(INTn_vect) {
    if (rcSignalValid == 1) { // Start with first pulse
        rcValues[rcSignalCounter] = TCNT0; // Store pulse length
    } else {
        rcSignalValid = 1; // First pulse received, start!
    }
    TCNT0 = RC_TIMER_RELOAD; // Reset Timer
}

void rcInit(void) {
    TIMSK0 |= (1 << TOIE0); // Enable Overflow Interrupt
    TCNT0 = RC_TIMER_RELOAD; // Overflow after 3,68ms
    TCCR0B |= (1 << CS02); // Prescaler 256

    // Select matching External Interrupt Control Register
#if RC_EXTINT < 4
    volatile uint8_t *extIntReg = &EICRA;
#elif RC_EXTINT < 8
    volatile uint8_t *extIntReg = &EICRB;
#else
#error EXTINT too high!
#endif

    *extIntReg |= (1 << ISCn0) | (1 << ISCn1); // Trigger on rising edge
    EIMSK |= (1 << INTn); // Enable external interrupt
}

/** @} */
