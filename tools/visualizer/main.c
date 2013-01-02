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

#define VERSION      "0.1"
#define HEADING      "Visualizer Version "VERSION // Is this really portable?
#define SUBHEADING   "Press q to exit!"
#define CONNECTED    "Connected"
#define DISCONNECTED "Disconnected"
#define HEADINGSIZE 4

// Main Menu
#define IDLE 0
#define MAINREDRAW 1
#define QUIT 2
#define SELECT 3
#define CONNECT 4

// Connect Menu
//#define IDLE 0
//#define MAINREDRAW 1
//#define QUIT 2

int width, height, connected = 0;
char *port = NULL;

void printHeading(char *third) {
    attron(A_BOLD | A_UNDERLINE | COLOR_PAIR(1));
    //mvprintw(0, (width - strlen(HEADING)) / 2, "%s", HEADING);
    mvprintw(0, 0, "%s", HEADING);
    attroff(A_BOLD | A_UNDERLINE | COLOR_PAIR(1));
    //mvprintw(1, (width - strlen(SUBHEADING)) / 2, "%s", SUBHEADING);
    mvprintw(1, (strlen(HEADING) - strlen(SUBHEADING)) / 2, "%s", SUBHEADING);
    //mvprintw(2, (width - strlen(third)) / 2, "%s", third);
    mvprintw(2, (strlen(HEADING) - strlen(third)) / 2, "%s", third);

    if (connected) {
        mvprintw(3, (strlen(HEADING) - strlen(CONNECTED)) / 2, CONNECTED);
    } else {
        mvprintw(3, (strlen(HEADING) - strlen(DISCONNECTED)) / 2, DISCONNECTED);
    }
}

int selectMenu(void) {
    char **ports = getSerialPorts();
    int i = 0;
    clear();
    printHeading("Port Selection");
    while (ports[i] != NULL) {
        mvprintw(i + HEADINGSIZE + 1, 0, "%i) %s", i, ports[i]);
        i++;
    }
    int choice = -1;
    do {
        mvprintw(i + HEADINGSIZE + 2, 0, "Your choice: ");
        clrtoeol();
        echo();
        scanw("%d", &choice);
        noecho();
        if (!((choice >= 0) && (choice < i))) {
            mvprintw(i + HEADINGSIZE + 3, 0, "Invalid!");
        }
    } while (!((choice >= 0) && (choice < i)));
    port = ports[choice];
    for (int a = 0; a < i; a++) {
        if (a != choice)
            free(ports[a]);
    }
    free(ports);
    return MAINREDRAW;
}

int connectMenu(void) {
    int ch, menu = MAINREDRAW;

    do {
        if (menu == MAINREDRAW) {
            clear();
            printHeading("Connection Menu");
            mvprintw(HEADINGSIZE + 1, 0, "c) Connect");
            mvprintw(HEADINGSIZE + 2, 0, "d) Disconnect");
            mvprintw(HEADINGSIZE + 3, 0, "q) Back");
            menu = IDLE;
        }
        refresh();
        ch = getch();
        switch (ch) {
            case 'c': case 'C':
                connected = 1;
                menu = MAINREDRAW;
                break;

            case 'd': case 'D':
                connected = 0;
                menu = MAINREDRAW;
                break;

            case 'q': case 'Q':
                menu = QUIT;
                break;
        }
    } while (menu != QUIT);

    return MAINREDRAW;
}

int main(int argc, char *argv[]) {
    int ch, menu = MAINREDRAW;

    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    getmaxyx(stdscr, height, width);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    char **ports = getSerialPorts();
    port = ports[0];
    ch = 1;
    while (ports[ch] != NULL) {
        free(ports[ch++]);
    }
    free(ports);

    do {
        if (menu == MAINREDRAW) {
            clear();
            printHeading("Main Menu");
            mvprintw(HEADINGSIZE + 1, 0, "s) Select Serial Port (current: %s)", port);
            mvprintw(HEADINGSIZE + 2, 0, "c) Connect");
            mvprintw(HEADINGSIZE + 3, 0, "q) Quit Visualizer");
            menu = IDLE;
        }
        refresh();
        ch = getch();
        switch (ch) {
            case 's': case 'S':
                menu = selectMenu();
                break;

            case 'c': case 'C':
                menu = connectMenu();
                break;

            case 'q': case 'Q':
                menu = QUIT;
                break;
        }
    } while (menu != QUIT);

    endwin();
    printf("Visualizer - %s - by Thomas Buck\n", VERSION);
    printf("Visit http://www.xythobuz.org\n");
    return 0;
}
