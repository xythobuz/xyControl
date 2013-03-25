/*
 * motor.h
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
#ifndef _motor_h
#define _motor_h

#include <config.h>

/** \addtogroup motor Motor Controller Driver
 *  \ingroup Hardware
 *  Controlling four <a href="https://www.mikrocontroller.com/index.php?main_page=product_info&products_id=209">BL-Ctrl V1.2</a> Brushless controllers.
 *  @{
 */

/** \file motor.h
 *  BL-Ctrl V1.2 Controller API Header.
 */

extern uint8_t motorSpeed[MOTORCOUNT]; /**< Speed for the four motors */

/** Initializes the motor control library.
 *  Really only sets motorSpeed to zero.
 */
void motorInit(void);

/** Set the speed of one or all motors.
 *  \param id Motor ID (0 to 3, 4 = all)
 *  \param speed New Speed
 */
void motorSet(uint8_t id, uint8_t speed);

/** Send the values stored in #motorSpeed to the Controllers.
 *  I2C already has to be initialized!
 */
void motorTask(void);

#endif
/** @} */
