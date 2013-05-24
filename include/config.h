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

/** \addtogroup config Configuration
 *  Various default settings.
 *  @{
 */

 /** \file config.h
 *  Various default settings.
 */

//  -----------------
// |    Filtering    |
//  -----------------

#define FILTER_KALMAN 0
#define FILTER_COMPLEMENTARY 1
#define ORIENTATION_FILTER FILTER_KALMAN /**< Filter Implementation to be used */

/** Time Contant for Low and High Pass Filter in the Complementary Filter.
 *  In essence, time periods shorter than TAU come from gyro data,
 *  longer time periods come from the Accelerometer data.
 *  In seconds!
 */
#define COMPLEMENTARY_TAU 0.5

// 1 has no effect, bigger numbers dampen the signal more
#define SOFTWARELOWPASS 1 /**< Software Low-Pass on Gyro and ACC */
#define ACCFILTERFACTOR SOFTWARELOWPASS /**< Accelerometer Low Pass Factor */
#define GYROFILTERFACTOR SOFTWARELOWPASS /**< Gyroscope Low Pass Factor */

#define PID_OUTMAX 256 /**< Maximum PID Output */
#define PID_OUTMIN -256 /**< Minimum PID Output */
#define PID_INTMAX PID_OUTMAX /**< Maximum PID Integral Sum */
#define PID_INTMIN PID_OUTMIN /**< Minimal PID Integral Sum */

//  -----------------
// |    Kalman      |
//  -----------------

/** Time Constant */
#define DT 0.01f // 100Hz

// Q (3x3 Matrix) with these elements on diagonal
#define Q1 5.0f /**< Q Matrix Diagonal Element 1 */
#define Q2 100.0f /**< Q Matrix Diagonal Element 2 */
#define Q3 0.01f /**< Q Matrix Diagonal Element 3 */

// R (2x2 Matrix) with these elements on diagonal
#define R1 1000.0f /**< R Matrix Diagonal Element 1 */
#define R2 1000.0f /**< R Matrix Diagonal Element 2 */

//  -----------------
// |       Set       |
//  -----------------

#define SET_ROLLPLUS 1 /**< Second Motor at the Right */
#define SET_ROLLMINUS 3 /**< Fourth Motor at the Left */
#define SET_PITCHPLUS 0 /**< First Motor at the Top */
#define SET_PITCHMINUS 2 /**< Third Motor at the Bottom */

//  -----------------
// |       PID       |
//  -----------------

#define PID_P 5.0 /**< Default PID P Constant */
#define PID_I 0.03 /**< Default PID I Constant */
#define PID_D -13.0 /**< Default PID D Constant */

//  -----------------
// |      Motor      |
//  -----------------

#define MOTORCOUNT 4  /**< Amount of motors */

//  -----------------
// |       ADC       |
//  -----------------

#define BATT_MAX 15 /**< Battery Voltage Reference (ADC 5V) */
#define BATT_CHANNEL 0 /**< ADC Channel for Battery */

//  -----------------
// |       TWI       |
//  -----------------

#define ACC_ADDRESS 0x32 /**< Accelerometer Address (0011001r) */
#define GYRO_ADDRESS 0xD6 /**< Gyroscope Address (110101xr, x = 1) */
#define MAG_ADDRESS 0x3C /**< Magnetometer Address */
#define MOTOR_BASEADDRESS 0x52 /**< Address of first motor controller */

//  -----------------
// |    xyControl    |
//  -----------------

#define LED0PORT PORTL /**< First LED Port */
#define LED0DDR DDRL /**< First LED Data Direction Register */
#define LED0PIN PL6 /**< First LED Pin */
#define LED1PORT PORTL /**< Second LED Port */
#define LED1DDR DDRL /**< Second LED Data Direction Register */
#define LED1PIN PL7 /**< Second LED Pin */
#define LED2PORT PORTG /**< Third LED Port */
#define LED2DDR DDRG /**< Third LED Data Direction Register */
#define LED2PIN PG5 /**< Third LED Pin */
#define LED3PORT PORTE /**< Fourth LED Port */
#define LED3DDR DDRE /**< Fourth LED Data Direction Register */
#define LED3PIN PE2 /**< Fourth LED Pin */

//  -----------------
// |      XMEM       |
//  -----------------

#define BANK0PORT PORTG /**< First Bank Selection Port */
#define BANK0DDR DDRG   /**< First Bank Selection Data Direction Register */
#define BANK0PIN PG3    /**< First Bank Selection Pin */
#define BANK1PORT PORTG /**< Second Bank Selection Port */
#define BANK1DDR DDRG   /**< Second Bank Selection Data Direction Register */
#define BANK1PIN PG4    /**< Second Bank Selection Pin */
#define BANK2PORT PORTL /**< Third Bank Selection Port */
#define BANK2DDR DDRL   /**< Third Bank Selection Data Direction Register */
#define BANK2PIN PL5    /**< Third Bank Selection Pin */

//  -----------------
// |       SPI       |
//  -----------------

#define SPIDDR DDRB /** SPI Data Direction Register */
#define SPIMOSI PB2 /** SPI MOSI Pin */
#define SPISCK PB1 /** SPI SCK Pin */
#define SPISS PB0 /**< SPI Slave Select Pin */

//  -----------------
// |     Serial      |
//  -----------------

#define RX_BUFFER_SIZE 64 /**< UART Receive Buffer Size */
#define TX_BUFFER_SIZE 64 /**< UART Transmit Buffer Size */

#endif
/** @} */
