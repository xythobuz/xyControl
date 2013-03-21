/*
 * xmem.c
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

#include <xmem.h>
#include <config.h>

typedef struct {
    char *start;
    char *end;
    void *val;
    void *fl;
} MallocState;

MallocState states[MEMBANKS];
uint8_t currentBank = 0;

extern void *__brkval;
extern void *__flp; // Internal Malloc State

void saveState(uint8_t bank);
void restoreState(uint8_t bank);

void saveState(uint8_t bank) {
    states[bank].start = __malloc_heap_start;
    states[bank].end = __malloc_heap_end;
    states[bank].val = __brkval;
    states[bank].fl = __flp;
}

void restoreState(uint8_t bank) {
    __malloc_heap_start = states[bank].start;
    __malloc_heap_end = states[bank].end;
    __brkval = states[bank].val;
    __flp = states[bank].fl;
}

void xmemInit(void) {
    BANK0DDR |= (1 << BANK0PIN);
    BANK1DDR |= (1 << BANK1PIN);
    BANK2DDR |= (1 << BANK2PIN);
    BANK0PORT &= ~(1 << BANK0PIN);
    BANK1PORT &= ~(1 << BANK1PIN);
    BANK2PORT &= ~(1 << BANK2PIN);

    XMCRB = 0; // Use full address space
    XMCRA = (1 << SRW11) | (1 << SRW10); // 3 Wait cycles
    XMCRA |= (1 << SRE); // Enable XMEM

    for (uint8_t i = 0; i < MEMBANKS; i++) {
        saveState(i);
    }
}

void xmemSetBank(uint8_t bank) {
    if (bank < MEMBANKS) {
        saveState(currentBank);

        BANK0PORT &= ~(1 << BANK0PIN);
        BANK1PORT &= ~(1 << BANK1PIN);
        BANK2PORT &= ~(1 << BANK2PIN);
        BANK0PORT |= ((bank & 0x01) << BANK0PIN);
        BANK1PORT |= (((bank & 0x02) >> 1) << BANK1PIN);
        BANK2PORT |= (((bank & 0x04) >> 2) << BANK2PIN);

        currentBank = bank;
        restoreState(bank);
    }
}

uint8_t xmemGetBank(void) {
    return currentBank;
}
