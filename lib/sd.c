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
#include <stdlib.h>

#include <spi.h>
#include <sd.h>
#include <xmem.h>
#include <config.h>

#define SELECT() (SD_CSPORT &= ~(1 << SD_CSPIN))
#define DESELECT() (SD_CSPORT |= (1 << SD_CSPIN))

#define writeByte(x) spiSendByte(x)
#define readByte() writeByte(0xFF)

uint8_t sdBlocksize;
uint32_t sdCardsize;

uint8_t sdCommand(uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
                    uint8_t cmd4, uint8_t cmd5, uint8_t cmd6) {
    uint8_t try = 0, v = 0xFF;

    DESELECT();
    writeByte(0xFF); // Sync
    SELECT();
    writeByte(cmd1);
    writeByte(cmd2);
    writeByte(cmd3);
    writeByte(cmd4);
    writeByte(cmd5);
    writeByte(cmd6);
    while (v == 0xFF) {
        v = readByte();
        if (try++ >= SD_TIMEOUT) {
            break;
        }
    }
    return v;
}

uint8_t readSomething(uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
                    uint8_t cmd4, uint8_t cmd5, uint8_t cmd6,
                    uint8_t *data, uint16_t length) {
    uint8_t v = sdCommand(cmd1, cmd2, cmd3, cmd4, cmd5, cmd6);
    if (v != 0) {
        return v;
    }
    while (readByte() != 0xFE); // Wait for start byte
    for (uint16_t i = 0; i < length; i++) {
        data[i] = readByte();
    }
    readByte();
    readByte(); // Read CRC
    DESELECT();
    return 0;
}

void readInfos(void) {
    uint8_t oldBank = xmemGetBank();
    xmemSetBank(BANK_GENERIC);
    uint8_t *csd = (uint8_t *)malloc(16);

    readSomething(0x49, 0x00, 0x00, 0x00, 0x00, 0xFF, csd, 16);
    sdBlocksize = csd[5] & 0x0F;

    // BLOCKNR = (C_SIZE + 1) * 2^(C_SIZE_MULT + 2)
    // CARDSIZE = BLOCKNR * BLOCKLEN
    uint16_t csize = (csd[8] & 0xC0) >> 6;
    csize |= (csd[7] << 2);
    uint8_t csizemult = (csd[10] & 0x80) >> 7;
    csizemult |= ((csd[9] & 0x03) << 1);
    sdCardsize = (1 << sdBlocksize) * (csize + 1) * (1 << (csizemult + 2));

    free(csd);
    xmemSetBank(oldBank);
}

// External API

uint8_t sdInit(void) {
    SD_CSDDR |= (1 << SD_CSPIN);
    SELECT();
    spiInit(SPI_MODE0, SPI_SPEED128); // 125kHz

    // Send some clocks to the card
    for (uint8_t i = 0; i < (SD_TIMEOUT / 7); i++) {
        writeByte(0xFF);
    }

    // Send CMD0 --> SPI Mode
    uint8_t try = 0;
    while (sdCommand(0x40, 0x00, 0x00, 0x00, 0x00, 0x95) != 1) {
        if (try++ >= SD_TIMEOUT) {
            DESELECT();
            return 1;
        }
    }

    spiInit(SPI_MODE0, SPI_SPEED2); // 8MHz
    DESELECT();
    readInfos();
    return 0;
}

uint16_t sdBlockSize(void) {
    return (uint16_t)(1 << sdBlocksize);
}

uint32_t sdCardSize(void) {
    return sdCardsize;
}

uint8_t sdReadBlock(uint32_t address, uint8_t *data) { // Block address
    address <<= sdBlocksize; // Convert address
    return readSomething(0x51, ((address & 0xFF000000) >> 24),
                        ((address & 0x00FF0000) >> 16),
                        ((address & 0x0000FF00) >> 8),
                        (address & 0x000000FF), 0xFF, data, sdBlockSize());
}

uint8_t sdWriteBlock(uint32_t address, uint8_t *data) { // Block address
    address <<= sdBlocksize; // Convert address
    uint8_t v = sdCommand(0x58, ((address & 0xFF000000) >> 24),
                                ((address & 0x00FF0000) >> 16),
                                ((address & 0x0000FF00) >> 8),
                                (address & 0x000000FF), 0xFF);
    if (v != 0) {
        return v;
    }
    for (uint8_t i = 0; i < SD_TIMEOUT; i++) {
        readByte(); // Send some clocks
    }
    writeByte(0xFE); // Start Byte
    for (uint16_t i = 0; i < sdBlockSize(); i++) {
        writeByte(data[i]); // Send data
    }
    writeByte(0xFF);
    writeByte(0xFF); // Dummy CRC
    if ((readByte() & 0x1F) != 0x05) {
        return 1; // Error while writing
    }
    while (readByte() != 0xFF); // Wait for Card
    DESELECT();
    return 0;
}
