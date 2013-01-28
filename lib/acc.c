/*
 * acc.c
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

#include <twi.h>
#include <acc.h>
#include <config.h>

#define DEBUG 0
#include <debug.h>

#if DEBUG >= 2
#include <stdio.h>
#endif

#define ACCREG_CTRL1 0x20
#define ACCREG_CTRL4 0x23
#define ACCREG_XL 0x28

void accWriteRegister(uint8_t reg, uint8_t val) {
#if DEBUG >= 2
    printf("  ACC: 0x%x to 0x%x...", val, reg);
#endif
    if (twiStart(ACC_ADDRESS | TWI_WRITE)) {
#if DEBUG >= 2
        printf(" No Answer!\n");
#endif
    }
    if (twiWrite(reg)) {
#if DEBUG >= 2
        printf(" Could not write 1\n");
#endif
    }
    if (twiWrite(val)) {
#if DEBUG >= 2
        printf(" Could not write 2\n");
#endif
    }
    twiStop();
#if DEBUG >= 2
    printf(" Done!\n");
#endif
}

uint8_t accInit(AccRange r) {
    debugPrint("Acc Init...\n");
    accWriteRegister(ACCREG_CTRL1, 0x27); // Enable all axes, 10Hz
    switch (r) {
        case r2G:
            accWriteRegister(ACCREG_CTRL4, 0x00);
            break;
        case r4G:
            accWriteRegister(ACCREG_CTRL4, 0x10);
            break;
        case r8G:
            accWriteRegister(ACCREG_CTRL4, 0x20);
            break;
        case r16G:
            accWriteRegister(ACCREG_CTRL4, 0x30);
            break;
    }
    debugPrint("Acc Initialized!\n");
    return 0;
}

void accRead(Vector *v) {
    twiStart(ACC_ADDRESS | TWI_WRITE);
    twiWrite(ACCREG_XL | (1 << 7)); // Auto Increment
    twiRepStart(ACC_ADDRESS | TWI_READ);
    uint8_t xl = twiReadAck();
    uint8_t xh = twiReadAck();
    uint8_t yl = twiReadAck();
    uint8_t yh = twiReadAck();
    uint8_t zl = twiReadAck();
    uint8_t zh = twiReadNak();

    v->x = ((int16_t)(xh << 8 | xl)) >> 4;
    v->y = ((int16_t)(yh << 8 | yl)) >> 4;
    v->z = ((int16_t)(zh << 8 | zl)) >> 4;
}
