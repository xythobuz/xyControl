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

/*
 * This aims to be a small walk through the inner
 * workings of the task scheduler and other library
 * features.
 */

/*
 * Function definitions
 */
void ledTask(void);
void printVoltage(void);

/*
 * Strings for UART menu, stored in Flash.
 */
char voltageString[] PROGMEM = "Battery Voltage";

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
     * This will toggle all LEDs
     */
    xyLed(LED_GREEN, LED_OFF);
    xyLed(LED_RED, LED_ON);
    addTask(&ledTask);

    /*
     * ADC Test, reading the battery voltage.
     * A new command for the UART menu is registered.
     */
    addMenuCommand('v', voltageString, &printVoltage);

    /*
     * Initialize the MinIMU9-v2 Accelerometer, Gyroscope and
     * Magnetometer and the Motor Task.
     */
    accInit(r2G);
    gyroInit(r250DPS);
    magInit(r1g9);
    motorInit();

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
