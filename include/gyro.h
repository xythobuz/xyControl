/*
 * gyro.h
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
#ifndef _gyro_h
#define _gyro_h

#include <error.h>
#include <xycontrol.h>

/** \addtogroup gyro Gyroscope Driver
 *  \ingroup Hardware
 *  Configuring and reading an L3GD20
 *  @{
 */

/** \file gyro.h
 * L3GD20 Gyroscope API Header
 */

/** Gyroscope Range options */
typedef enum {
    r250DPS, /**< +- 250 Degrees per Second */
    r500DPS, /**< +- 500 Degrees per Second */
    r2000DPS, /**< +- 2000 Degrees per Second */
} GyroRange;

/** Initializes the Gyroscope.
 * I2C should already be initialized.
 * \param r #GyroRange to use
 * \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS
 */
Error gyroInit(GyroRange r);

/** Get a set of gyroscope data.
 * gyroInit() should already be called.
 * \param v Data Destionation
 * \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS
 */
Error gyroRead(Vector3f *v);

#endif
/** @} */
