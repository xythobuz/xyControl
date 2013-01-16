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
#include <avr/pgmspace.h>

#include <xycontrol.h>
#include <xmem.h>
#include <tasks.h>
#include <serial.h>
#include <uartMenu.h>

typedef struct MenuEntry MenuEntry;
struct MenuEntry{
    char cmd;
    PGM_P helpText;
    Task f;
    MenuEntry *next;
};

MenuEntry *uartMenu = NULL;

MenuEntry *findEntry(char cmd) {
    MenuEntry *p = uartMenu;
    while (p != NULL) {
        if (p->cmd == cmd) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

uint8_t addMenuCommand(char cmd, PGM_P help, Task f) {
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

void uartMenuPrintHelp(void) {
    uint8_t lastBank = xmemGetBank();
    xmemSetBank(BANK_GENERIC);
    char *buffer = (char *)malloc(35);
    if (buffer == NULL) {
        serialWrite('!');
        return;
    }
    MenuEntry *p = uartMenu;
    while (p != NULL) {
        serialWrite(p->cmd);
        serialWrite(':');
        serialWrite(' ');
        strcpy_P(buffer, p->helpText);
        serialWriteString(buffer);
        serialWrite('\n');
        p = p->next;
    }
    free(buffer);
    xmemSetBank(lastBank);
}

void uartMenuTask(void) {
    if (serialHasChar()) {
        uint8_t lastBank = xmemGetBank();
        xmemSetBank(BANK_GENERIC);
        char c = serialGet();
        MenuEntry *p = uartMenu;
        while (p != NULL) {
            if (p->cmd == c) {
                p->f();
                xmemSetBank(lastBank);
                return;
            }
            p = p->next;
        }
        serialWrite('?');
        xmemSetBank(lastBank);
    }
}
