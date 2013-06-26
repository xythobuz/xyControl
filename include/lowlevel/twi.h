/*
 * twi.h
 *
 * Copyright (c) 2005, Peter Fleury <pfleury@gmx.ch>
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
#ifndef _twi_h
#define _twi_h

/** \addtogroup twi I2C Driver
 *  \ingroup Hardware
 *  Using the AVR TWI/I2C Hardware
 *  @{
 */

/** \file twi.h
 *  I2C API Header.
 */

#define TWI_READ 1 /**< I2C Read Bit */
#define TWI_WRITE 0 /**< I2C Write Bit */

/** Initialize the I2C Hardware */
void twiInit(void);

/** Stop the I2C Hardware */
void twiStop(void);

/** Start an I2C Transfer.
 *  \param addr Slave Address (with Read/Write bit)
 *  \returns 0 on success, 1 on error
 */
unsigned char twiStart(unsigned char addr);

/** Start a repeated I2C Transfer.
 *  \param addr Slave Address (with Read/Write bit)
 *  \returns 0 on success, 1 on error
 */
unsigned char twiRepStart(unsigned char addr);

/** Start an I2C Transfer and poll until ready.
 *  \param addr Slave Address (with Read/Write bit)
 */
void twiStartWait(unsigned char addr);

/** Write to the I2C Slave.
 *  \param data Data to send
 *  \returns 0 on success, 1 on error
 */
unsigned char twiWrite(unsigned char data);

/** Read from the I2C Slave and request more data.
 *  \returns Data read
 */
unsigned char twiReadAck(void);

/** Read from the I2C Slave and deny more data.
 *  \returns Data read
 */
unsigned char twiReadNak(void);

#endif
/** @} */
