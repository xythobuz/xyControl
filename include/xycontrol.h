/*
 * xycontrol.h
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
#ifndef _xycontrol_h
#define _xycontrol_h

/** \addtogroup xycontrol xyControl Hardware
 *  \ingroup System
 *  Controls xyControl On-Board Hardware like LEDs.
 *  @{
 */

/** \file xycontrol.h
 *  xyControl API Header.
 */

/** Methods of addressing the LEDs */
typedef enum {
	LED_RED0 = 0, /**< First red LED */
	LED_RED1 = 1, /**< Second red LED */
	LED_GREEN0 = 2, /**< First green LED */
	LED_GREEN1 = 3, /**< Second green LED */
	LED_ALL = 4, /**< All LEDs */
	LED_BITMAP = 5, /**< LEDs as Bitmap (R0, R1, G0, G1) */
	LED_RED = 6, /**< Both red LEDs */
	LED_GREEN = 7 /**< Both green LEDs */
} LED;

/** Possible states of the LEDs */
typedef enum {
	LED_OFF = 0, /**< LED Off */
	LED_ON = 1, /**< LED On */
	LED_TOGGLE = 2 /**< Toggle the LED */
} LEDState;

/** The global 3-Dimensional Floating Point Vector. */
typedef struct {
    double x; /**< X Part */
  	double y; /**< Y Part */
  	double z; /**< Z Part */
} Vector3f;

/** Initialize the xyControl Hardware.
 *  Initializes LEDs, Timer, UART, I2C, SPI, ADC, the UART Menu
 *  and prepares stdin and stdout.
 */
void xyInit(void);

/** Set the LEDs.
 *  \param l LEDs to set
 *  \param v New LED State
 */
void xyLed(LED l, LEDState v);

/** Calculate and return the Battery Voltage.
 *  \returns Current Battery Voltage
 */
double getVoltage(void);

/** Use the Watchdog to reset yourself after 15ms. */
void xySelfReset(void);

#endif
/** @} */
