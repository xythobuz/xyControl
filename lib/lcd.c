/*
 * lcd.c
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
#include <stdint.h>
#include <avr/io.h>

#include <lowlevel/serial.h>
#include <lcd.h>

/** \addtogroup lcd UART LCD interface
 *  \ingroup Hardware
 *  @{
 */

/** \file lcd.c
 *  UART LCD API Implementation
 */

#define LCD_SPECIALCOMMAND 0x7C

#define LCD_CLEAR 0x01
#define LCD_CURSORRIGHT 0x14
#define LCD_CURSORLEFT 0x10
#define LCD_SCROLLRIGHT 0x1C
#define LCD_SCROLLLEFT 0x18
#define LCD_TURNOFF 0x08
#define LCD_TURNON 0x0C
#define LCD_UNDERLINEON 0x0E
#define LCD_UNDERLINEOFF 0x0C
#define LCD_BLINKON 0x0D
#define LCD_BLINKOFF 0x0C

#define LCD_COMMAND 0xFE
#define LCD_SETPOSITION 0x80

#define LCD_BRIGHTNESSMIN 0x80
#define LCD_BRIGHTNESSMAX 0x9D

#define LCD_LINES 4
#define LCD_WIDTH 20

#define LCD_LINE1 0
#define LCD_LINE2 64
#define LCD_LINE3 20
#define LCD_LINE4 84

void lcdClear(void) {
    serialWrite(DISPLAY, LCD_COMMAND);
    serialWrite(DISPLAY, LCD_CLEAR);
}

void lcdBrightness(uint8_t val) {
    if (val > (LCD_BRIGHTNESSMAX - LCD_BRIGHTNESSMIN))
        val = LCD_BRIGHTNESSMAX;
    else
        val += LCD_BRIGHTNESSMIN;
    serialWrite(DISPLAY, LCD_SPECIALCOMMAND);
    serialWrite(DISPLAY, val);
}

void lcdPosition(uint8_t x, uint8_t y) {
    if ((x < LCD_WIDTH) && (y < LCD_LINES)) {
        uint8_t pos = 0;
        switch (y) {
            case 0:
                pos = LCD_LINE1;
                break;
            case 1:
                pos = LCD_LINE2;
                break;
            case 2:
                pos = LCD_LINE3;
                break;
            case 3:
                pos = LCD_LINE4;
                break;
        }
        pos += x;
        pos |= LCD_SETPOSITION;
        serialWrite(DISPLAY, LCD_COMMAND);
        serialWrite(DISPLAY, pos);
    }
}

void lcdChar(char c) {
    serialWrite(DISPLAY, c);
}

void lcdString(const char *data) {
    serialWriteString(DISPLAY, data);
}

/** @} */
