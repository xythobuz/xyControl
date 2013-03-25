/*
 * uartMenu.h
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
#ifndef _uartMenu_h
#define _uartMenu_h

#include <tasks.h>

/** \addtogroup uartmenu UART Menu
 *  \ingroup System
 *  Enables user interaction with an UART Menu.
 *  @{
 */

/** \file uartMenu.h
 *  UART Menu API Header.
 */

typedef struct MenuEntry MenuEntry;
/** Data Structure for Single-Linked-List for UART Menu.
 * Stores Helptext, command and action.
 */
struct MenuEntry {
    uint8_t cmd; /**< Byte that triggers the action */
    PGM_P helpText; /**< Text (in Flash) printed with help command */
    Task f; /**< Action that get's executed */
    MenuEntry *next; /**< Next MenuEntry in the linked list */
};

/** Add a command to the UART Menu. 
 *  \param cmd Byte that triggers command
 *  \param help Help Text String in Flash
 *  \param f Task to be executed
 *  \return 0 on success, 1 if already registered or not enough memory.
 */
uint8_t addMenuCommand(uint8_t cmd, PGM_P help, Task f); // 0 on success

/** Print all registered commands. */
void uartMenuPrintHelp(void);

/** Register a Handler for unhandled menu commands.
 *  \param handler Will be called if an unknown command is received.
 */
void uartMenuRegisterHandler(void (*handler)(char)); // Gets called when no command

/** Task to work the UART Menu. */
void uartMenuTask(void);

#endif
/** @} */
