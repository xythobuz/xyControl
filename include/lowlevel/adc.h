/*
 * adc.h
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
#ifndef _adc_h
#define _adc_h

/** \addtogroup adc ADC Driver
 *  \ingroup Hardware
 *  Analog-to-Digital Converter Library.
 *  With 10bit Output and selectable Reference Voltage.
 *  @{
 */

/** \file adc.h
 *  Analog-to-Digital Converter API Header
 */

/** ADC Reference Voltage options. */
typedef enum {
	AREF, /**< External Reference Voltage */
	AVCC, /**< Supply Voltage */
	AINT1, /**< Internal Reference 1 (1.1V) */
	AINT2 /**< Internal Reference 2 (2.56V) */
} ADCRef;

/** Initialize the ADC Hardware.
 * \param ref Reference Voltage.
 */
void adcInit(ADCRef ref);

/** Start a conversion on a given channel.
 * \param channel Channel (0 - 15)
 */
void adcStart(uint8_t channel);

/** Check if a result is ready.
 * \returns 1 if conversion is done.
 */
uint8_t adcReady(void);

/** Get the conversion results.
 * \param next Start next conversion if != 0
 * \returns 10bit ADC value
 */
uint16_t adcGet(uint8_t next);

/** Disable the ADC to save energy. */
void adcClose(void);

#endif
/** @} */
