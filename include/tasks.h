/*
 * tasks.h
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
#ifndef _tasks_h
#define _tasks_h

/** \addtogroup task Task Handler
 *  \ingroup System
 *  System for registering different tasks that will be called regularly,
 *  one after another.
 *  @{
 */

/** \file tasks.h
 *  Task API Header.
 */

typedef void (*Task)(void); /**< A Task has no arguments and returns nothing. */

typedef struct TaskElement TaskElement;
/** Single-Linked Task List. */
struct TaskElement {
    Task task; /**< Task to be executed */
    TaskElement *next; /**< Next list element */
};

extern TaskElement *taskList; /**< List of registered Tasks */

/** Adds another task that will be called regularly.
 *  \param func Task to be executed
 *  \returns 0 on success
 */
uint8_t addTask(Task func);

/** Removes an already registered Task.
 *  \param func Task to be removed
 *  \returns 0 on success
 */
uint8_t removeTask(Task func);

/** Executes registered Tasks.
 *  Call this in your Main Loop!
 */
void tasks(void);

/** Get the number of registered Tasks.
 *  \returns Count of registered Tasks
 */
uint8_t tasksRegistered(void);

#endif
/** @} */
