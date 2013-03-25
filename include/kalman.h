/*
 * kalman.h
 *
 * Copyright Linus Helgesson
 * http://www.linushelgesson.se/2012/04/pitch-and-roll-estimating-kalman-filter-for-stabilizing-quadrocopters/
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
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
#ifndef _kalman_h
#define _kalman_h

/** \addtogroup kalman Kalman-Filter
 *  \ingroup Flight
 *  Kalman-Filter from <a href="http://www.linushelgesson.se/2012/04/pitch-and-roll-estimating-kalman-filter-for-stabilizing-quadrocopters/">Linus Helgesson</a>
 *  
 *  @{
 */

/** \file kalman.h
 *  Kalman-Filter Header.
 */

/** Kalman-Filter State data */
typedef struct {
    double x1, x2, x3; /**< X Vector */
    double p11, p12, p13, p21, p22, p23, p31, p32, p33; /**< P Matrix */
} Kalman;

/** Step the Kalman Filter.
 *  \param data Kalman-Filter State
 *  \param z1 Angle from Accelerometer
 *  \param z2 Corresponding Gyroscope data
 */
void kalmanInnovate(Kalman *data, double z1, double z2);

/** Initialize a Kalman-State.
 *  \param data Kalman-State to be initialized
 */
void kalmanInit(Kalman *data);

#endif
/** @} */
