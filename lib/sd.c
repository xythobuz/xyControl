/*
 * sd.c
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

#include <spi.h>
#include <sd.h>
#include <config.h>

#define SELECT() (SD_CSPORT &= ~(1 << SD_CSPIN))
#define DESELECT() (SD_CSPORT |= (1 << SD_CSPIN))

#define writeByte(x) spiSendByte(x)
#define readByte() writeByte(0xFF)

// Internal API

uint8_t sdCommand(uint8_t *cmd) {
    uint8_t try = 0, v = 0xFF;

    DESELECT();
    writeByte(0xFF); // Sync
    SELECT();
    for (uint8_t i = 0; i < SD_CMDSIZE; i++) {
        writeByte(cmd[i]);
    }
    while (v == 0xFF) {
        v = readByte();
        if (try++ >= SD_TIMEOUT) {
            break;
        }
    }
    return v;
}

// External API

uint8_t sdInit(void) {
    SD_CSDDR |= (1 << SD_CSPIN);
    SELECT();
    spiInit(SPI_MODE0, SPI_SPEED128); // 125kHz

    // Send some clocks to the card
    for (uint8_t i = 0; i < 0x10; i++) {
        writeByte(0xFF);
    }

    // Send CMD0 --> SPI Mode
    uint8_t cmd[SD_CMDSIZE] = { 0x40, 0x00, 0x00, 0x00, 0x00, 0x95 };
    uint8_t try = 0;
    while (sdCommand(cmd) != 1) {
        if (try++ >= SD_TIMEOUT) {
            DESELECT();
            return 1;
        }
    }

    spiInit(SPI_MODE0, SPI_SPEED2); // 8MHz
    DESELECT();
    return 0;
}

uint16_t sdBlockSize(void) {
    return 512;
}

uint8_t sdReadBlock(uint32_t address, uint8_t *data) {
    return 0;
}

uint8_t sdWriteBlock(uint32_t address, uint8_t *data) {
    return 0;
}
