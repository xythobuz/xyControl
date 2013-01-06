/*
 * mag.c
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
#include <mag.h>

#define MAGREG_CRA 0x00
#define MAGREG_CRB 0x01
#define MAGREG_MR 0x02
#define MAGREG_XH 0x03
#define MAGREG_TEMPH 0x31
#define MAGREG_TEMPL 0x32

void magWriteRegister(uint8_t reg, uint8_t val) {
    twiStart(MAG_ADDRESS | TWI_WRITE);
    twiWrite(reg);
    twiWrite(val);
    twiStop();
}

uint8_t magInit(MagRange r) {
    magWriteRegister(MAGREG_MR, 0x00); // Continuous Conversion
    magWriteRegister(MAGREG_CRB, (r << 5)); // Set Range
    return 0;
}

void magRead(Vector *v) {
    twiStart(MAG_ADDRESS | TWI_WRITE);
    twiWrite(MAGREG_XH);
    twiRepStart(MAG_ADDRESS | TWI_READ);
    uint8_t xh = twiReadAck();
    uint8_t xl = twiReadAck();
    uint8_t zh = twiReadAck();
    uint8_t zl = twiReadAck();
    uint8_t yh = twiReadAck();
    uint8_t yl = twiReadNak();

    v->x = (int16_t)(xh << 8 | xl);
    v->y = (int16_t)(yh << 8 | yl);
    v->z = (int16_t)(zh << 8 | zl);
}
