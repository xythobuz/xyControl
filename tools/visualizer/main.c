/*
 * main.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "serial.h"

#define VERSION    "0.1"
#define HEADING    "Visualizer - "VERSION // Is this really portable?
#define SUBHEADING "Press q to exit!"

#define MAINREDRAW 0
#define IDLE 1
#define QUIT 2
#define SELECT 3
#define CONNECT 4

int width, height;

void printHeading() {
    attron(A_BOLD | A_UNDERLINE | COLOR_PAIR(1));
    mvprintw(0, (width - strlen(HEADING)) / 2, "%s", HEADING);
    attroff(A_BOLD | A_UNDERLINE | COLOR_PAIR(1));
    mvprintw(1, (width - strlen(SUBHEADING)) / 2, "%s", SUBHEADING);
}

int selectMenu(char **portVar) {
    char **ports = getSerialPorts();
    int i = 0;
    clear();
    printHeading();
    while (ports[i] != NULL) {
        mvprintw(i + 3, 0, "%i) %s", i, ports[i]);
        i++;
    }
    int choice = -1;
    do {
        mvprintw(i + 4, 0, "Your choice: ");
        clrtoeol();
        echo();
        scanw("%d", &choice);
        noecho();
        if (!((choice >= 0) && (choice < i))) {
            mvprintw(i + 5, 0, "Invalid!");
        }
    } while (!((choice >= 0) && (choice < i)));
    *portVar = ports[choice];
    for (int a = 0; a < i; a++) {
        if (a != choice)
            free(ports[a]);
    }
    free(ports);
    return MAINREDRAW;
}

int main(int argc, char *argv[]) {
    int ch, menu = MAINREDRAW;
    char *port = NULL;

    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, height, width);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    do {
        if (menu == MAINREDRAW) {
            clear();
            printHeading();
            mvprintw(3, 0, "0)  S  -  Select Serial Port (current: %s)", port);
            mvprintw(4, 0, "1)  C  -  Connect");
            mvprintw(5, 0, "2)  Q  -  Quit Visualizer");
            menu = IDLE;
        }
        refresh();
        ch = getch();
        switch (ch) {
            case 's': case 'S': case '0':
                menu = selectMenu(&port);
                break;

            case 'c': case 'C': case '1':

                break;

            case 'q': case 'Q': case '2':
                menu = QUIT;
                break;
        }
    } while (menu != QUIT);

    endwin();
    printf("Visualizer - %s - by Thomas Buck\n", VERSION);
    printf("Visit http://www.xythobuz.org\n");
    return 0;
}
