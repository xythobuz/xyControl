/*
 * orientation.c
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
#include <avr/io.h>
#include <stdint.h>
#include <math.h>

#include <xycontrol.h>
#include <error.h>
#include <gyro.h>
#include <acc.h>
#include <mag.h>
#include <tasks.h>
#include <time.h>
#include <orientation.h>
#include <kalman.h>
#include <complementary.h>
#include <config.h>

/** \addtogroup orientation Orientation Calculation
 *  \ingroup Flight
 *  @{
 */

/** \file orientation.c
 *  Orientation API Implementation.
 */

#define TODEG(x) ((x * 180) / M_PI) /**< Convert Radians to Degrees */

/** Current Aircraft orientation */
Angles orientation = {.pitch = 0, .roll = 0, .yaw = 0};

/** Current Aircraft orientation offset */
Angles orientationError = {.pitch = 0, .roll = 0, .yaw = 0};

#if ORIENTATION_FILTER == FILTER_KALMAN
Kalman pitchData; /**< Kalman-State for Pitch Angle */
Kalman rollData; /**< Kalman-State for Roll Angle */
#elif ORIENTATION_FILTER == FILTER_COMPLEMENTARY
Complementary pitchData; /**< Complementary-State for Pitch Angle */
Complementary rollData; /**< Complementary-State for Roll Angle */
#else
#error Define a Filter for the orientation Data
#endif

Error orientationInit(void) {
    Error e = accInit(r4G);
    CHECKERROR(e);
    e = gyroInit(r250DPS);
    CHECKERROR(e);

#if ORIENTATION_FILTER == FILTER_KALMAN
    kalmanInit(&pitchData);
    kalmanInit(&rollData);
#elif ORIENTATION_FILTER == FILTER_COMPLEMENTARY
    complementaryInit(&pitchData);
    complementaryInit(&rollData);
#endif

    return SUCCESS;
}

Error orientationTask(void) {
    Vector3f g, a, m;
    Error e = accRead(&a); // Read Accelerometer
    CHECKERROR(e);
    e = gyroRead(&g); // Read Gyroscope
    CHECKERROR(e);

    // Calculate Pitch & Roll from Accelerometer Data
    double roll = atan(a.x / hypot(a.y, a.z));
    double pitch = atan(a.y / hypot(a.x, a.z));
    roll = TODEG(roll);
    pitch = TODEG(pitch); // As Degree, not radians!

    // Filter Roll and Pitch with Gyroscope Data from the corresponding axis
#if ORIENTATION_FILTER == FILTER_KALMAN
    kalmanInnovate(&pitchData, pitch, g.x);
    kalmanInnovate(&rollData, roll, g.y);
    orientation.roll = rollData.x1;
    orientation.pitch = pitchData.x1;
#elif ORIENTATION_FILTER == FILTER_COMPLEMENTARY
    complementaryExecute(&pitchData, pitch, g.x);
    complementaryExecute(&rollData, roll, g.y);
    orientation.roll = rollData.angle;
    orientation.pitch = pitchData.angle;
#endif

    // Zero Offset for angles
    orientation.roll -= orientationError.roll;
    orientation.pitch -= orientationError.pitch;

    return SUCCESS;
}

void zeroOrientation(void) {
    orientationError.roll = orientation.roll + orientationError.roll;
    orientationError.pitch = orientation.pitch + orientationError.pitch;
    orientationError.yaw = orientation.yaw + orientationError.yaw;
}
/** @} */
