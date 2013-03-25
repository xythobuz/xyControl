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
#include <stdlib.h>

#include <twi.h>
#include <mag.h>
#include <error.h>
#include <config.h>

/** \addtogroup mag
 *  \ingroup Hardware
 *  @{
 */

/** \file mag.c
 *  LSM303DLHC Magnetometer API Implementation.
 */

#define MAGREG_CRB 0x01 /**< Magnetometer Gain Register */
#define MAGREG_MR 0x02 /**< Magnetometer Mode Register */
#define MAGREG_XH 0x03 /**< First Magnetometer Output Register */

/** Write a Magnetometer Register.
 * I2C should aready be initialized!
 *
 * \param reg Register Address
 * \param val New Value
 * \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR or #SUCCESS.
 */
Error magWriteRegister(uint8_t reg, uint8_t val) {
    if (twiStart(MAG_ADDRESS | TWI_WRITE)) {
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

Error magInit(MagRange r) {
    if ((r <= 0) || (r >= 8)) {
        return ARGUMENT_ERROR;
    }
    Error e = magWriteRegister(MAGREG_MR, 0x00); // Continuous Conversion
    if (e != SUCCESS) {
        return e;
    }
    e = magWriteRegister(MAGREG_CRB, (r << 5)); // Set Range
    return e;
}

Error magRead(Vector3f *v) {
    if (v == NULL) {
        return ARGUMENT_ERROR;
    }
    if (twiStart(MAG_ADDRESS | TWI_WRITE)) {
        return TWI_NO_ANSWER;
    }
    if (twiWrite(MAGREG_XH)) {
        return TWI_WRITE_ERROR;
    }
    if (twiRepStart(MAG_ADDRESS | TWI_READ)) {
        return TWI_NO_ANSWER;
    }
    uint8_t xh = twiReadAck();
    uint8_t xl = twiReadAck();
    uint8_t zh = twiReadAck();
    uint8_t zl = twiReadAck();
    uint8_t yh = twiReadAck();
    uint8_t yl = twiReadNak();

    v->x = (int16_t)(xh << 8 | xl);
    v->y = (int16_t)(yh << 8 | yl);
    v->z = (int16_t)(zh << 8 | zl);
    return SUCCESS;
}
/** @} */
