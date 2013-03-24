/*
 * acc.h
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
#ifndef _acc_h
#define _acc_h

#include <error.h>
#include <xycontrol.h>

/** \file acc.h
 *  LSM303DLHC Accelerometer API.
 */

/** Accelerometer Range options */
typedef enum {
    r2G, /**< +- 2G */
    r4G, /**< +- 4G */
    r8G, /**< +- 8G */
    r16G, /**< +- 16G */
} AccRange;

/** Initialize the Accelerometer.
 * Call before accRead(). I2C should already be initialized!
 *
 * \param r #AccRange to use.
 * \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS.
 */
Error accInit(AccRange r);

/** Read from the Accelerometer.
 * Accelerometer should already be initialized!
 * \param v #Vector3f for the read values
 * \returns #TWI_NO_ANSWER, #TWI_WRITE_ERROR, #ARGUMENT_ERROR or #SUCCESS.
 */
Error accRead(Vector3f *v);

#endif
