/*
 * hardwareTest.c
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
#include <stdlib.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <tasks.h>
#include <xycontrol.h>
#include <time.h>
#include <uartMenu.h>
#include <serial.h>
#include <acc.h>
#include <gyro.h>
#include <mag.h>
#include <motor.h>
#include <orientation.h>
#include <xmem.h>
#include <error.h>

/** \example hardwareTest.c
 * Small walk-through the inner workings of the task scheduler
 * and other library features.
 */

void ledTask(void);
void printVoltage(void);
void printRaw(void);
void ramTest(void);
void bluetoothTest(void);

/*
 * Strings for UART menu, stored in Flash.
 */
char PROGMEM voltageString[] = "Battery Voltage";
char PROGMEM sensorString[] = "Raw Sensor Data";
char PROGMEM ramString[] = "Test external RAM";
char PROGMEM bluetoothString[] = "Test Bluetooth Module";

int main(void) {

    /*
     * Initialize the System Timer, UART, TWI, SPI,
     * ADC and the UART menu task for user or software
     * interaction. Also enables interrupts!
     * Also, the UART will be tied to stdin, stdout and stderr.
     * This allows you to use stdio.h utilities like printf()
     */
    xyInit();
    printf("Initializing Hardware Test...\n");

    /*
     * Initialize Hardware
     */
    xyLed(LED_GREEN, LED_OFF);
    xyLed(LED_RED, LED_ON);
    motorInit();
    orientationInit();

    /*
     * Register Tasks in the Scheduler. A UART task
     * is already registered...
     */
    addTask(&ledTask); // Blink LED

    /*
     * Add commands for the UART menu
     */
    addMenuCommand('b', bluetoothString, &bluetoothTest);
    addMenuCommand('r', sensorString, &printRaw);
    addMenuCommand('t', ramString, &ramTest);
    addMenuCommand('v', voltageString, &printVoltage);

    printf("Hardware Test Initialized!\n");

    /*
     * Execute all registered tasks, forever.
     */
    for(;;) {
        tasks();
    }

    return 0;
}

void ledTask(void) {
    /*
     * Basic example of executing a task with a given frequency.
     * last contains the last time this task was executed.
     */
    static time_t last = 0;
    if ((getSystemTime() - last) > 125) { // 125ms have passed
        xyLed(LED_ALL, LED_TOGGLE); // Do something...
        last = getSystemTime(); // Store new execution time
    }
}

void printVoltage(void) {
    printf("Battery: %fV\n", getVoltage());
}

void printRaw(void) {
    Vector3f v;
    accRead(&v);
    printf("Ax: %f Ay: %f Az: %f\n", v.x, v.y, v.z);
    gyroRead(&v);
    printf("Gx: %f Gy: %f Gz: %f\n", v.x, v.y, v.z);
    magRead(&v);
    printf("Mx: %f My: %f Mz: %f\n", v.x, v.y, v.z);
}

#define CHECKSIZE 53248 // 52KB

void ramTest(void) {
    uint8_t *blocks[MEMBANKS];
    uint8_t oldBank = xmemGetBank();

    printf("Allocating Test Memory...\n");
    for (uint8_t i = 0; i < MEMBANKS; i++) {
        xmemSetBank(i);
        blocks[i] = (uint8_t *)malloc(CHECKSIZE);
        if (blocks[i] == NULL) {
            printf("  Error: Couldn't allocate %liKB in Bank %i!\n", (CHECKSIZE / 1024), i);
        } else {
            printf("  Bank %i ready!\n", i);
        }
    }
    printf("Filling with data...\n");
    for (uint8_t i = 0; i < MEMBANKS; i++) {
        xmemSetBank(i);
        for (uint16_t j = 0; j < CHECKSIZE; j++) {
            blocks[i][j] = (j & 0xFF);
        }
        printf("  Filled Bank %i!\n", i);
    }
    printf("Checking data...\n");
    for (uint8_t i = 0; i < MEMBANKS; i++) {
        xmemSetBank(i);
        uint8_t error = 0;
        for (uint16_t j = 0; ((j < CHECKSIZE) && (!error)); j++) {
            if (blocks[i][j] != (j & 0xFF)) {
                printf("  Error at %i in %i!\n", j, i);
                error = 1;
            }
        }
        if (!error) {
            printf("  Bank %i okay!\n", i);
        }
    }
    printf("Freeing memory...\n");
    for (uint8_t i = 0; i < MEMBANKS; i++) {
        xmemSetBank(i);
        free(blocks[i]);
    }
    printf("Finished!\n");

    xmemSetBank(oldBank);
}

void bluetoothTest(void) {
    printf("Please disconnect, wait 10s, then reconnect!\n");
    printf("All data will be logged, then printed after 15s.\n");
    time_t start = getSystemTime();
    while ((getSystemTime() - start) <= 15000); // Wait
    while (serialHasChar(BLUETOOTH)) { // Check
        serialWrite(USB, serialGet(BLUETOOTH));
    }
    printf("\n\nDone!\n");
}