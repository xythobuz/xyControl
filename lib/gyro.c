/*
 * gyro.c
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

#include <twi.h>
#include <gyro.h>
#include <error.h>
#include <config.h>

#define GYROREG_CTRL1 0x20
#define GYROREG_CTRL4 0x23
#define GYROREG_OUTXL 0x28

GyroRange gyroRange;

Error gyroWriteByte(uint8_t reg, uint8_t val) {
    if (twiStart(GYRO_ADDRESS | TWI_WRITE)) {
        return TWI_NO_ANSWER;
    }
    if (twiWrite(reg)) {
        return TWI_WRITE_ERROR;
    }
    if (twiWrite(val)) {
        return TWI_WRITE_ERROR;
    }
    twiStop();
    return SUCCESS;
}

Error gyroInit(GyroRange r) {
    uint8_t v;
    switch (r) {
        case r250DPS:
            v = 0x00;
            break;
        case r500DPS:
            v = 0x10;
            break;
        case r2000DPS:
            v = 0x20;
            break;
        default:
            return ARGUMENT_ERROR;
    }
    gyroRange = r;
    Error e = gyroWriteByte(GYROREG_CTRL1, 0x0F);
    if (e != SUCCESS) {
        return e;
    }
    e = gyroWriteByte(GYROREG_CTRL4, v);
    return e;
}

Error gyroRead(Vector *v) {
    if (v == NULL) {
        return ARGUMENT_ERROR;
    }
    if (twiStart(GYRO_ADDRESS | TWI_WRITE)) {
        return TWI_NO_ANSWER;
    }
    if (twiWrite(GYROREG_OUTXL | 0x80)) { // Auto Increment
        return TWI_WRITE_ERROR;
    }
    if (twiRepStart(GYRO_ADDRESS | TWI_READ)) {
        return TWI_NO_ANSWER;
    }

    uint8_t xl = twiReadAck();
    uint8_t xh = twiReadAck();
    uint8_t yl = twiReadAck();
    uint8_t yh = twiReadAck();
    uint8_t zl = twiReadAck();
    uint8_t zh = twiReadNak();

    int16_t x = *(int8_t *)(&xh);
    x *= (1 << 8);
    x |= xl;

    int16_t y = *(int8_t *)(&yh);
    y *= (1 << 8);
    y |= yl;

    int16_t z = *(int8_t *)(&zh);
    z *= (1 << 8);
    z |= zl;

    switch (gyroRange) {
        case r250DPS:
            v->x = (((double)x) * 250 / 0x8000);
            v->y = (((double)y) * 250 / 0x8000);
            v->z = (((double)z) * 250 / 0x8000);
            break;
        case r500DPS:
            v->x = (((double)x) * 500 / 0x8000);
            v->y = (((double)y) * 500 / 0x8000);
            v->z = (((double)z) * 500 / 0x8000);
            break;
        case r2000DPS:
            v->x = (((double)x) * 2000 / 0x8000);
            v->y = (((double)y) * 2000 / 0x8000);
            v->z = (((double)z) * 2000 / 0x8000);
            break;
        default:
            return ARGUMENT_ERROR;
    }

    return SUCCESS;
}
