/*
 * config.h
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
#ifndef _config_h
#define _config_h

//  -----------------
// |    Filtering    |
//  -----------------

#define SOFTWARELOWPASS 25
#define ACCFILTERFACTOR SOFTWARELOWPASS
#define GYROFILTERFACTOR SOFTWARELOWPASS
#define PIDDIVISOR 2


//  -----------------
// |       Set       |
//  -----------------

#define SET_ROLLPLUS 1
#define SET_ROLLMINUS 3
#define SET_PITCHPLUS 0
#define SET_PITCHMINUS 2

//  -----------------
// |       PID       |
//  -----------------

#define PID_P 5.0
#define PID_I 0.03
#define PID_D -13.0

//  -----------------
// |      Motor      |
//  -----------------

#define MOTORCOUNT 4

//  -----------------
// |       ADC       |
//  -----------------

#define BATT_MAX 15 // 5V at ADC --> 15V Battery
#define BATT_CHANNEL 0

//  -----------------
// |       TWI       |
//  -----------------

#define ACC_ADDRESS 0x32 // 0011001r
#define GYRO_ADDRESS 0xD6 // 110101xr, x = 1
#define MAG_ADDRESS 0x3C
#define MOTOR_BASEADDRESS 0x52

//  -----------------
// |     MMC/SD      |
//  -----------------

#define SD_CSPORT PORTJ
#define SD_CSDDR DDRJ
#define SD_CSPIN PJ7
#define SD_TIMEOUT 100

//  -----------------
// |    xyControl    |
//  -----------------

#define LED0PORT PORTL
#define LED0DDR DDRL
#define LED0PIN PL6
#define LED1PORT PORTL
#define LED1DDR DDRL
#define LED1PIN PL7
#define LED2PORT PORTG
#define LED2DDR DDRG
#define LED2PIN PG5
#define LED3PORT PORTE
#define LED3DDR DDRE
#define LED3PIN PE2


//  -----------------
// |      XMEM       |
//  -----------------

#define BANK0PORT PORTG
#define BANK0DDR DDRG
#define BANK0PIN PG3
#define BANK1PORT PORTG
#define BANK1DDR DDRG
#define BANK1PIN PG4
#define BANK2PORT PORTL
#define BANK2DDR DDRL
#define BANK2PIN PL5


//  -----------------
// |       SPI       |
//  -----------------

#define SPIDDR DDRB
#define SPIMOSI PB2
#define SPISCK PB1
#define SPISS PB0


//  -----------------
// |     Serial      |
//  -----------------

// If you define this, a '\r' (CR) will be put in front of '\n' (LF)
// #define SERIALINJECTCR

// Defining this enables incoming XON XOFF (sends XOFF if rx buff is full)
// #define FLOWCONTROL

#define RX_BUFFER_SIZE 128
#define TX_BUFFER_SIZE 128

// Select the used UART module, from 0 to 3
#define UART 1

#endif
