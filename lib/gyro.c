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

#define GYRO_ID 0xD4
#define GYROREG_CTRL1 0x20
#define GYROREG_CTRL4 0x23
#define GYROREG_OUTXL 0x28
#define GYROREG_WHOAMI 0x0F

GyroRange range;

uint8_t gyroReadByte(uint8_t reg) {
    twiStart(GYRO_ADDRESS | TWI_WRITE);
    twiWrite(reg);
    twiRepStart(GYRO_ADDRESS | TWI_READ);
    // uint8_t val = twiReadNak();
    uint8_t val = twiReadAck();
    twiStop();
    return val;
}

void gyroWriteByte(uint8_t reg, uint8_t val) {
    twiStart(GYRO_ADDRESS | TWI_WRITE);
    twiWrite(reg);
    twiWrite(val);
    twiStop();
}

uint8_t gyroInit(GyroRange r) {
    range = r;
    if (gyroReadByte(GYROREG_WHOAMI) != GYRO_ID) {
        return 1;
    }
    gyroWriteByte(GYROREG_CTRL1, 0x0F);
    switch (range) {
        case r250DPS:
            gyroWriteByte(GYROREG_CTRL4, 0x00);
            break;
        case r500DPS:
            gyroWriteByte(GYROREG_CTRL4, 0x10);
            break;
        case r2000DPS:
            gyroWriteByte(GYROREG_CTRL4, 0x20);
            break;
    }
    return 0;
}

/*
 * Bit 00-15: X-Value
 * Bit 16-31: Y-Value
 * Bit 32-47: Z-Value
 * Bit 48-63: Zero
 */
uint64_t gyroRead(void) {
    twiStart(GYRO_ADDRESS | TWI_WRITE);
    twiWrite(GYROREG_OUTXL | 0x80); // Auto Increment
    twiRepStart(GYRO_ADDRESS | TWI_READ);
    uint8_t xl = twiReadAck();
    uint8_t xh = twiReadAck();
    uint8_t yl = twiReadAck();
    uint8_t yh = twiReadAck();
    uint8_t zl = twiReadAck();
    // uint8_t zh = twiReadNak();
    uint8_t zh = twiReadAck();
    twiStop();

    uint32_t x = (xl | (xh << 8));
    uint32_t y = (yl | (yh << 8));
    uint32_t z = (zl | (zh << 8));

    switch (range) {
        case r250DPS:
            x *= 22;
            x = (x >> 8); // x /= 256
            y *= 22;
            y = (y >> 8); // y /= 256
            z *= 22;
            z = (z >> 8); // z /= 256
            break;
        case r500DPS:
            x *= 45;
            x = (x >> 8); // x /= 256
            y *= 45;
            y = (y >> 8); // y /= 256
            z *= 45;
            z = (z >> 8); // z /= 256
            break;
        case r2000DPS:
            x *= 18;
            x = (x >> 8); // x /= 256
            y *= 18;
            y = (y >> 8); // y /= 256
            z *= 18;
            z = (z >> 8); // z /= 256
            break;
    }

    uint64_t result = (x & 0xFFFF);
    result |= ((y & 0xFFFF) << 16);
    result |= ((uint64_t)(z & 0xFFFF) << 32);

    return result;
}
