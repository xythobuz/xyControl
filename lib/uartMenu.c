/*
 * uartMenu.c
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
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include <xycontrol.h>
#include <xmem.h>
#include <tasks.h>
#include <serial.h>
#include <uartMenu.h>

/** \addtogroup uartmenu UART Menu
 *  \ingroup System
 *  @{
 */

/** \file uartMenu.c
 *  UART Menu API Implementation.
 */

MenuEntry *uartMenu = NULL; /**< Single-Linked-List for commands */
void (*unHandler)(char) = NULL; /**< Handler for unhandled commands */

/** Search the #uartMenu Linked List.
 *  \param cmd Command to search for
 *  \returns MenuEntry for command cmd, or NULL
 */
MenuEntry *findEntry(uint8_t cmd) {
    MenuEntry *p = uartMenu;
    while (p != NULL) {
        if (p->cmd == cmd) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

uint8_t addMenuCommand(uint8_t cmd, PGM_P help, Task f) {
    uint8_t lastBank = xmemGetBank();
    xmemSetBank(BANK_GENERIC);
    if (findEntry(cmd) != NULL) {
        return 1;
    } else {
        MenuEntry *p = (MenuEntry *)malloc(sizeof(MenuEntry));
        if (p == NULL) {
            return 1;
        }
        p->cmd = cmd;
        p->helpText = help;
        p->f = f;
        p->next = uartMenu;
        uartMenu = p;
        return 0;
    }
    xmemSetBank(lastBank);
}

/** Reverse the UART Menu List.
 *  \param root Root of the Single-Linked-List.
 *  \returns New root of reversed list.
 */
MenuEntry *reverseList(MenuEntry *root) {
    MenuEntry *new = NULL;
    while (root != NULL) {
        MenuEntry *next = root->next;
        root->next = new;
        new = root;
        root = next;
    }
    return new;
}

void uartMenuPrintHelp(void) {
    static uint8_t reversed = 0;
    uint8_t lastBank = xmemGetBank();
    xmemSetBank(BANK_GENERIC);
    char *buffer = (char *)malloc(35);
    if (buffer == NULL) {
        printf("!");
        return;
    }
    if (!reversed) {
        reversed = 1;
        uartMenu = reverseList(uartMenu);
    }
    MenuEntry *p = uartMenu;
    while (p != NULL) {
        strcpy_P(buffer, p->helpText);
        printf("%c: %s\n", p->cmd, buffer);
        p = p->next;
    }
    free(buffer);
    xmemSetBank(lastBank);
}

void uartMenuRegisterHandler(void (*handler)(char)) {
    unHandler = handler;
}

void uartMenuTask(void) {
    for (uint8_t i = 0; i < serialAvailable(); i++) {
        if (serialHasChar(i)) {
            uint8_t lastBank = xmemGetBank();
            xmemSetBank(BANK_GENERIC);
            uint8_t c = serialGet(i);
            MenuEntry *p = uartMenu;
            while (p != NULL) {
                if (p->cmd == c) {
                    p->f();
                    xmemSetBank(lastBank);
                    return;
                }
                p = p->next;
            }
            if (unHandler != NULL)
                unHandler(c);
            xmemSetBank(lastBank);
        }
    }
}
/** @} */
