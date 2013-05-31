/*
 * orientation.h
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

#ifndef _orientation_h
#define _orientation_h

#include <error.h>

/** \addtogroup orientation Orientation Calculation
 *  \ingroup Flight
 *  Calculate Orientation using the Kalman-Filter, Accelerometer and Gyroscope.
 *
 *  @{
 */

/** \file orientation.h
 *  Orientation API Header.
 */

/** Can store orientation in Euler Space */
typedef struct {
    double pitch; /**< Pitch Angle in Degrees */
    double roll; /**< Roll Angle in Degrees */
    double yaw; /**< Yaw Angle in Degrees */
    double vPitch; /**< Pitch Angle Speed in Degrees per Second */
    double vRoll; /**< Roll Angle Speed in Degrees per Second */
    double vYaw; /**< Yaw Angle Speed in Degrees per Second */
} Angles;

extern Angles orientation; /**< Current Aircraft orientation */

/** Initializes the Orientation API.
 *  Also initializes the Accelerometer, Gyroscope and Magnetometer.
 *  I2C should already be initialized!
 *  \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS.
 */
Error orientationInit(void);

/** Calculate the current orientation.
 *  It will be stored in the global #orientation Struct.
 *  \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS.
 */
Error orientationTask(void);

/** Sets the current orientation to zero. */
void zeroOrientation(void);

#endif
/** @} */
