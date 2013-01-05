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
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "serial.h"

#define BAUD 38400

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

// Connect Menu
//#define IDLE 0
//#define MAINREDRAW 1
//#define QUIT 2

void printHeading(char *third);
int selectMenu(void);
int connect(void);
void requestData(int *data, int length, char c);
void drawRectangle(int x, int y, int width, int height);
int getPercentage(int d);
int showData(void);
void intHandler(int dummy);

int width, height, fd = -1;
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

    if (fd != -1) {
        mvprintw(3, (strlen(HEADING) - strlen(CONNECTED)) / 2, CONNECTED);
    } else {
        mvprintw(3, (strlen(HEADING) - strlen(DISCONNECTED)) / 2, DISCONNECTED);
    }
}

int selectMenu(void) {
    // You should have less than (TerminalHeight - 5) serial ports... :/
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

int connect(void) {
    if ((fd == -1) && (port != NULL)) {
        fd = serialOpen(port, BAUD, 0, 0, 0); // flow = 0, vmin = 0, vtime = 0
    } else if (fd != -1) {
        serialClose(fd);
        fd = -1;
    }
    return MAINREDRAW;
}

void requestData(int *data, int length, char c) {
    serialWriteChar(fd, c);
    for (int i = 0; i < length; i++) {
        char ch[2];
        for (int j = 0; j < 2; j++) {
            time_t start = time(NULL);
            while (!serialHasChar(fd)) {
                if ((time(NULL) - start) > 0) {
                    printf("Timeout!");
                    return;
                }
            }
            serialReadChar(fd, ch + j);
        }
        data[i] = (ch[0] << 8) | ch[1];
    }
}

void drawRectangle(int x, int y, int width, int height) {
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width + 1, ACS_URCORNER);
    mvaddch(y + height + 1, x, ACS_LLCORNER);
    mvaddch(y + height + 1, x + width + 1, ACS_LRCORNER);
    for (int i = (x + 1); i < (x + width + 1); i++) {
        mvaddch(y, i, ACS_HLINE);
        mvaddch(y + height + 1, i, ACS_HLINE);
    }
    for (int i = (y + 1); i < (y + height + 1); i++) {
        mvaddch(i, x, ACS_VLINE);
        mvaddch(i, x + width + 1, ACS_VLINE);
    }
}

int getPercentage(int d) {
    // Interpret d as 16bit 2-complement number
    // Return 50 (%) on Zero, Positive > 50, Negative < 50
    if (d == 0) {
        return 50;
    } else if (d <= 32768) {
        return 50 + (d / 655);
    } else if (d > 32768) {
        d -= 32769;
        return d / 655;
    }
}

int showData(void) {
    int menu = IDLE;
    int *data = (int *)malloc(3 * 3 * sizeof(int));

    clear();
    printHeading("Data");

    // Accelerometer & Gyroscope Drawings
    for (int i = HEADINGSIZE + 1; i <= HEADINGSIZE + 16; i += 3) {
        drawRectangle(3, i, 51, 1);
    }
    mvaddch(HEADINGSIZE + 2, 1, 'X');
    mvaddch(HEADINGSIZE + 5, 1, 'Y');
    mvaddch(HEADINGSIZE + 8, 1, 'Z');
    mvaddch(HEADINGSIZE + 11, 1, 'X');
    mvaddch(HEADINGSIZE + 14, 1, 'Y');
    mvaddch(HEADINGSIZE + 17, 1, 'Z');
    for (int i = 0; i < 3; i++) {
        mvaddch(HEADINGSIZE + 2 + (3 * i), 29, ACS_DIAMOND);
        mvaddch(HEADINGSIZE + 11 + (3 * i), 29, ACS_DIAMOND);
    }

    do {
        requestData(data, 3, 'a');
        requestData(data + 3, 3, 'g');
        //requestData(data + 6, 3, 'm');

        for (int i = 0; i < 6; i++) {
            int d = (int)((double)getPercentage(data[i]) / 2);
            mvaddch(HEADINGSIZE + 2 + (3 * i), 4 + d, ACS_CKBOARD);
        }

        int ch = getch();
        if (ch != ERR) {
            menu = QUIT;
        }
    } while (menu != QUIT);

    free(data);
    return MAINREDRAW;
}

int main(int argc, char *argv[]) {
    int ch, menu = MAINREDRAW;

    signal(SIGINT, intHandler);
    signal(SIGQUIT, intHandler);

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
    timeout(1); // Input blocks for 1ms

    char **ports = getSerialPorts();
    ch = 0;
    while (ports[ch] != NULL) {
        ch++;
    }
    port = ports[ch - 1];
    for (int i = 0; i < (ch - 1); i++) {
        free(ports[i]);
    }
    free(ports);

    do {
        if (menu == MAINREDRAW) {
            clear();
            printHeading("Main Menu");
            mvprintw(HEADINGSIZE + 1, 0, "s) Select Serial Port (current: %s)", port);
            if (fd == -1) {
                mvprintw(HEADINGSIZE + 2, 0, "c) Connect");
                mvprintw(HEADINGSIZE + 3, 0, "q) Quit Visualizer");
            } else {
                mvprintw(HEADINGSIZE + 2, 0, "d) Disconnect");
                mvprintw(HEADINGSIZE + 3, 0, "x) Show Data");
                mvprintw(HEADINGSIZE + 4, 0, "q) Quit Visualizer");
            }
            menu = IDLE;
        }
        refresh();
        ch = getch();
        switch (ch) {
            case 's': case 'S':
                menu = selectMenu();
                break;

            case 'c': case 'C': case 'd': case 'D':
                menu = connect();
                break;

            case 'x': case 'X':
                if (fd != -1) {
                    menu = showData();
                }
                break;

            case 'q': case 'Q':
                menu = QUIT;
                break;
        }
    } while (menu != QUIT);

    endwin();
    if (fd != -1) {
        serialClose(fd);
    }
    printf("Visualizer - %s - by Thomas Buck\n", VERSION);
    printf("Visit http://www.xythobuz.org\n");
    return 0;
}

void intHandler(int dummy) {
    endwin();
    printf("Exiting...\n");
    if (fd != -1) {
        serialClose(fd);
    }
    exit(1);
}
