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
#include <stdlib.h>

#include <twi.h>
#include <acc.h>
#include <error.h>
#include <stdlib.h>
#include <config.h>

/** \addtogroup acc
 *  \ingroup Hardware
 *  @{
 */

/** \file acc.c
 *  LSM303DLHC Accelerometer API Implementation.
 */

#define ACCREG_CTRL1 0x20 /**< Accelerometer Control Register 1 */
#define ACCREG_CTRL4 0x23 /**< Accelerometer Control Register 4 */
#define ACCREG_XL 0x28 /**< First Accelerometer Output Register */

AccRange accRange; /**< Stored range to scale returned values. */

/** Write an Accelerometer Register.
 * I2C should aready be initialized!
 *
 * \param reg Register Address
 * \param val New Value
 * \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR or #SUCCESS.
 */
Error accWriteRegister(uint8_t reg, uint8_t val) {
    if (twiStart(ACC_ADDRESS | TWI_WRITE)) {
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

Error accInit(AccRange r) {
    uint8_t v;
    switch (r) {
        case r2G:
            v = 0x00;
            break;
        case r4G:
            v = 0x10;
            break;
        case r8G:
            v = 0x20;
            break;
        case r16G:
            v = 0x30;
            break;
        default:
            return ARGUMENT_ERROR;
    }
    accRange = r;
    Error e = accWriteRegister(ACCREG_CTRL1, 0x57); // Enable all axes, 100Hz
    if (e != SUCCESS) {
        return e;
    }
    e = accWriteRegister(ACCREG_CTRL4, v);
    return e;
}

Error accRead(Vector3f *v) {
    static double accSumX = 0; /* Buffer for X Low-Pass. */
    static double accSumY = 0; /* Buffer for Y Low-Pass. */
    static double accSumZ = 0; /* Buffer for Z Low-Pass. */
    static double accFilterX = 0; /* Buffer for X Low-Pass. */
    static double accFilterY = 0; /* Buffer for Y Low-Pass. */
    static double accFilterZ = 0; /* Buffer for Z Low-Pass. */

    if (v == NULL) {
        return ARGUMENT_ERROR;
    }
    if (twiStart(ACC_ADDRESS | TWI_WRITE)) {
        return TWI_NO_ANSWER;
    }
    if (twiWrite(ACCREG_XL | (1 << 7))) { // Auto Increment
        return TWI_WRITE_ERROR;
    }
    if (twiRepStart(ACC_ADDRESS | TWI_READ)) {
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

    switch (accRange) {
        case r2G:
            v->x = (((double)x) * 2 / 0x8000);
            v->y = (((double)y) * 2 / 0x8000);
            v->z = (((double)z) * 2 / 0x8000);
            break;
        case r4G:
            v->x = (((double)x) * 4 / 0x8000);
            v->y = (((double)y) * 4 / 0x8000);
            v->z = (((double)z) * 4 / 0x8000);
            break;
        case r8G:
            v->x = (((double)x) * 8 / 0x8000);
            v->y = (((double)y) * 8 / 0x8000);
            v->z = (((double)z) * 8 / 0x8000);
            break;
        case r16G:
            v->x = (((double)x) * 16 / 0x8000);
            v->y = (((double)y) * 16 / 0x8000);
            v->z = (((double)z) * 16 / 0x8000);
            break;
        default:
            return ARGUMENT_ERROR;
    }

    accSumX = accSumX - accFilterX + v->x;
    accFilterX = accSumX / ACCFILTERFACTOR;
    v->x = accFilterX;

    accSumY = accSumY - accFilterY + v->y;
    accFilterY = accSumY / ACCFILTERFACTOR;
    v->y = accFilterY;

    accSumZ = accSumZ - accFilterZ + v->z;
    accFilterZ = accSumZ / ACCFILTERFACTOR;
    v->z = accFilterZ;

    return SUCCESS;
}
/** @} */
