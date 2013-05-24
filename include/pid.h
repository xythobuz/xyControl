/*
 * pid.h
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
#ifndef _pid_h
#define _pid_h

/** \addtogroup pid PID-Controller
 *  \ingroup Flight
 *  Simple implementation for multiple floating-point
 *  PID Controllers.
 *  @{
 */

/** \file pid.h
 *  PID Library Header
 */

/** Data Structure for a single PID Controller.
 *  Stores all needed constants and state variables.
 */
typedef struct {
    double kp; /**< Proportional factor. Default is #PID_P. */
    double ki; /**< Integral factor. Default is #PID_I. */
    double kd; /**< Derivative factor. Default is #PID_D. */
    double outMin; /**< Minimum Output. Default is #PID_OUTMIN. */
    double outMax; /**< Maximum Output. Default is #PID_OUTMAX. */
    double intMin; /**< Minimum Integral sum. Default is #PID_INTMIN. */
    double intMax; /**< Maximum Integral sum. Default is #PID_INTMAX. */
    double lastError; /**< Derivative State. */
    double sumError; /**< Integral state. Kept in #intMin, #intMax Range. */
    time_t last; /**< Last execution time. For dT calculation. */
} PIDState;

#define ROLL 0 /**< Roll index for #pidTarget, #pidOutput and #pidStates. */
#define PITCH 1 /**< Pitch index for #pidTarget, #pidOutput and #pidStates. */

extern double pidTarget[2]; /**< Roll and Pitch target angles. */
extern double pidOutput[2]; /**< Roll and Pitch PID Output. */
extern PIDState pidStates[2]; /**< Roll and Pitch PID States. */

/** Initialize Roll and Pitch PID.
 * Stores the PID States in #pidStates.
 * Also resets #pidTarget to zero.
 */
void pidInit(void);

/** Step the Roll and Pitch PID Controllers.
 * Placing their output in #pidOutput and reading the input
 * from #pidTarget and the global orientation Angles.
 */
void pidTask(void);

/** Set the parameters of a PID controller.
 *  The state variables will be reset to zero.
 *
 * \param pid PIDState to be changed.
 * \param kp New Proportional constant.
 * \param ki New Integral constant.
 * \param kd New Derivative constant.
 * \param min New minimum Output.
 * \param max New maximum Output.
 * \param iMin New minimal Integral Sum.
 * \param iMax New maximal Integral Sum.
 */
void pidSet(PIDState *pid, double kp, double ki, double kd, double min, double max, double iMin, double iMax);

/** Execute a single PID Control Step.
 * \param should Target value
 * \param is Measured value
 * \param state PID State
 * \return PID Output
 */
double pidExecute(double should, double is, PIDState *state);

#endif
/** @} */
