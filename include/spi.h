/*
 * spi.h
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
#ifndef _spi_h
#define _spi_h

/** \addtogroup spi SPI Driver
 *  \ingroup Hardware
 *  SPI Library for AVRs built-in SPI Hardware.
 *  @{
 */

/** \file spi.h
 *  SPI API Header
 */

/** SPI Mode option */
typedef enum {
    MODE_0 = 0, /**< CPOL 0, CPHA 0 */
    MODE_1 = 1, /**< CPOL 0, CPHA 1 */
    MODE_2 = 2, /**< CPOL 1, CPHA 0 */
    MODE_3 = 3, /**< CPOL 1, CPHA 1 */
} SPI_MODE;

/** SPI Speed options */
typedef enum {
    SPEED_2 = 4, /**< F_CPU / 2 */
    SPEED_4 = 0, /**< F_CPU / 4 */
    SPEED_8 = 5, /**< F_CPU / 8 */
    SPEED_16 = 1, /**< F_CPU / 16 */
    SPEED_32 = 6, /**< F_CPU / 32 */
    SPEED_64 = 2, /**< F_CPU / 64 */
    SPEED_128 = 3, /**< F_CPU / 128 */
} SPI_SPEED;

/** Initialize the SPI Hardware Module.
 *  \param mode SPI Mode to use
 *  \param speed SPI Speed to use
 */
void spiInit(SPI_MODE mode, SPI_SPEED speed);

/** Send and Receive one byte.
 *  Set the Chip Select Lines yourself!
 *  \param d Data to be sent
 *  \returns Byte read from Bus
 */
uint8_t spiSendByte(uint8_t d);

#endif
/** @} */
