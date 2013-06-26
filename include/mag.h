/*
 * mag.h
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
#ifndef _mag_h
#define _mag_h

#include <datatypes.h>

/** \addtogroup mag Magnetometer Driver
 *  \ingroup Hardware
 *  Configuring and reading an LSM303DLHC Magnetometer.
 *  @{
 */

/** \file mag.h
 *  LSM303DLHC Magnetometer API Header.
 */

/** Magnetometer Range options */
typedef enum {
    r1g3 = 1, /**< +- 1.3 Gauss */
    r1g9 = 2, /**< +- 1.9 Gauss */
    r2g5 = 3, /**< +- 2.5 Gauss */
    r4g0 = 4, /**< +- 4.0 Gauss */
    r4g7 = 5, /**< +- 4.7 Gauss */
    r5g6 = 6, /**< +- 5.6 Gauss */
    r8g1 = 7, /**< +- 8.1 Gauss */
} MagRange;

/** Initialize the Magnetometer.
 *  Call before magRead(). I2C should already be initialized!
 *
 *  \param r #MagRange to use.
 *  \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS.
 */
Error magInit(MagRange r);

/** Read from the Magnetometer.
 *  Magnetometer should already be initialized!
 *  \param v #Vector3f for the read values
 *  \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS.
 */
Error magRead(Vector3f *v);

#endif
/** @} */
