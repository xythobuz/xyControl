/*
 * serial.h
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
#ifndef _serial_h
#define _serial_h

/** \addtogroup uart UART Driver
 *  \ingroup Hardware
 *  Serial Library for AVRs built-in UART Hardware.
 *  Allows XON/XOFF Flow Control.
 *  FIFO Buffer for Receiving and Transmitting.
 *  @{
 */

/** \file serial.h
 *  UART API Header
 */

/** Calculate Baudrate Register Value */
#define BAUD(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/** Initialize the UART Hardware.
 *  \param baud Baudrate. Use the BAUD() macro!
 */
void serialInit(uint16_t baud);

/** Stop the UART Hardware */
void serialClose(void);

/** Manually change the flow control.
 *  \param on 1 of on, 0 if off
 */
void setFlow(uint8_t on);

/** Check if a byte was received.
 *  \returns 1 if a byte was received, 0 if not
 */
uint8_t serialHasChar(void);

/** Read a single byte.
 *  \returns Received byte or 0
 */
uint8_t serialGet(void);

/** Wait until a character is received.
 *  \returns Received byte
 */
uint8_t serialGetBlocking(void);

/** Check if the receive buffer is full.
 *  \returns 1 if buffer is full, 0 if not
 */
uint8_t serialRxBufferFull(void);

/** Check if the receive buffer is empty.
 *  \returns 1 if buffer is empty, 0 if not.
 */
uint8_t serialRxBufferEmpty(void);

/** Send a byte.
 *  \param data Byte to send
 */
void serialWrite(uint8_t data);

/** Send a string.
 *  \param data Null-Terminated String
 */
void serialWriteString(const char *data);

/** Check if the transmit buffer is full.
 *  \returns 1 if buffer is full, 0 if not
 */
uint8_t serialTxBufferFull(void);

/** Check if the transmit buffer is empty.
 *  \returns 1 if buffer is empty, 0 if not.
 */
uint8_t serialTxBufferEmpty(void);

#endif
/** @} */
