/*
 * serial.c
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
#include <avr/interrupt.h>
#include <stdint.h>

#include <serial.h>
#include <config.h>

/** \addtogroup uart UART Driver
 *  \ingroup Hardware
 *  @{
 */

/** \file serial.c
 *  UART API Implementation
 */

#define XON 0x11 /**< XON Byte */
#define XOFF 0x13 /**< XOFF Byte */

#if (RX_BUFFER_SIZE < 2) || (TX_BUFFER_SIZE < 2)
#error SERIAL BUFFER TOO SMALL!
#endif

#ifdef FLOWCONTROL
#if (RX_BUFFER_SIZE < 8) || (TX_BUFFER_SIZE < 8)
#error SERIAL BUFFER TOO SMALL!
#endif
#endif

#if (RX_BUFFER_SIZE + TX_BUFFER_SIZE) >= (RAMEND - 0x60)
#error SERIAL BUFFER TOO LARGE!
#endif

#define FLOWMARK 5 /**< Space remaining to trigger XOFF/XON */

// Macro concatenation magic --> selectable uart module
#define CAT_X(A, B, C) A ## B ## C
#define CAT(A, B, C) CAT_X(A, B, C)
#define CATB_X(A, B) A ## B
#define CATB(A, B) CATB_X(A, B)

// Register definitions
#define SERIALRECIEVEINTERRUPT      CAT(USART, UART, _RX_vect)
#define SERIALTRANSMITINTERRUPT     CAT(USART, UART, _UDRE_vect)
#define SERIALB                     CAT(UCSR, UART, B)
#define SERIALC                     CAT(UCSR, UART, C)
#define SERIALUPM1                  CAT(UPM, UART, 1)
#define SERIALUPM0                  CAT(UPM, UART, 0)
#define SERIALUCSZ0                 CAT(UCSZ, UART, 0)
#define SERIALUCSZ1                 CAT(UCSZ, UART, 1)
#define SERIALUCSZ2                 CAT(UCSZ, UART, 2)
#define SERIALA                     CAT(UCSR, UART, A)
#define SERIALDATA                  CATB(UDR, UART)
#define SERIALIE                    CATB(UDRIE, UART)
#define SERIALUSBS                  CATB(USBS, UART)
#define SERIALRXCIE                 CATB(RXCIE, UART)
#define SERIALRXEN                  CATB(RXEN, UART)
#define SERIALTXEN                  CATB(TXEN, UART)
#define SERIALUDRIE                 CATB(UDRIE, UART)
#define SERIALUDRE                  CATB(UDRE, UART)
#define SERIALUBRR                  CATB(UBRR, UART)

uint8_t volatile rxBuffer[RX_BUFFER_SIZE]; /**< RX FIFO Buffer */
uint8_t volatile txBuffer[TX_BUFFER_SIZE]; /**< TX FIFO Buffer */
uint16_t volatile rxRead = 0; /**< RX FIFO Read Position */
uint16_t volatile rxWrite = 0; /**< RX FIFO Write Position */
uint16_t volatile txRead = 0; /**< TX FIFO Read Position */
uint16_t volatile txWrite = 0; /**< TX FIFO Write Position */
uint8_t volatile shouldStartTransmission = 1; /**< Should enable interrupt */

#ifdef FLOWCONTROL
uint8_t volatile sendThisNext = 0; /**< Send char without FIFO */
uint8_t volatile flow = 1; /**< Flow Control Status */
uint8_t volatile rxBufferElements = 0; /**< Elements in RX FIFO Buffer */
#endif

/** Receive Complete Interrupt */
ISR(SERIALRECIEVEINTERRUPT) {
    rxBuffer[rxWrite] = SERIALDATA;
    if (rxWrite < (RX_BUFFER_SIZE - 1)) {
        rxWrite++;
    } else {
        rxWrite = 0;
    }

#ifdef FLOWCONTROL
    rxBufferElements++;
    if ((flow == 1) && (rxBufferElements >= (RX_BUFFER_SIZE - FLOWMARK))) {
        sendThisNext = XOFF;
        flow = 0;
        if (shouldStartTransmission) {
            shouldStartTransmission = 0;
            SERIALB |= (1 << SERIALUDRIE);
            SERIALA |= (1 << SERIALUDRE); // Trigger Interrupt
        }
    }
#endif
}

/** Data Register Empty Interrupt */
ISR(SERIALTRANSMITINTERRUPT) {
#ifdef FLOWCONTROL
    if (sendThisNext) {
        SERIALDATA = sendThisNext;
        sendThisNext = 0;
    } else {
#endif
        if (txRead != txWrite) {
            SERIALDATA = txBuffer[txRead];
            if (txRead < (TX_BUFFER_SIZE -1)) {
                txRead++;
            } else {
                txRead = 0;
            }
        } else {
            shouldStartTransmission = 1;
            SERIALB &= ~(1 << SERIALUDRIE); // Disable Interrupt
        }
#ifdef FLOWCONTROL
    }
#endif
}

void serialInit(uint16_t baud) {
    // Default: 8N1
    SERIALC = (1 << SERIALUCSZ0) | (1 << SERIALUCSZ1);

    // Set baudrate
    SERIALUBRR = baud;

    SERIALB = (1 << SERIALRXCIE); // Enable Interrupts
    SERIALB |= (1 << SERIALRXEN) | (1 << SERIALTXEN); // Enable Receiver/Transmitter
}

void serialClose(void) {
    uint8_t sreg = SREG;
    sei();
    while (!serialTxBufferEmpty());
    while (SERIALB & (1 << SERIALUDRIE)); // Wait while Transmit Interrupt is on
    cli();
    SERIALB = 0;
    SERIALC = 0;
    rxRead = 0;
    txRead = 0;
    rxWrite = 0;
    txWrite = 0;
    shouldStartTransmission = 1;
#ifdef FLOWCONTROL
    flow = 1;
    sendThisNext = 0;
    rxBufferElements = 0;
#endif
    SREG = sreg;
}

void setFlow(uint8_t on) {
#ifdef FLOWCONTROL
    if (flow != on) {
        if (on == 1) {
            // Send XON
            while (sendThisNext != 0);
            sendThisNext = XON;
            flow = 1;
            if (shouldStartTransmission) {
                shouldStartTransmission = 0;
                SERIALB |= (1 << SERIALUDRIE);
                SERIALA |= (1 << SERIALUDRE); // Trigger Interrupt
            }
        } else {
            // Send XOFF
            sendThisNext = XOFF;
            flow = 0;
            if (shouldStartTransmission) {
                shouldStartTransmission = 0;
                SERIALB |= (1 << SERIALUDRIE);
                SERIALA |= (1 << SERIALUDRE); // Trigger Interrupt
            }
        }
        // Wait till it's transmitted
        while (SERIALB & (1 << SERIALUDRIE));
    }
#endif
}

// ---------------------
// |     Reception     |
// ---------------------

uint8_t serialHasChar(void) {
    if (rxRead != rxWrite) { // True if char available
        return 1;
    } else {
        return 0;
    }
}

uint8_t serialGetBlocking(void) {
    while(!serialHasChar());
    return serialGet();
}

uint8_t serialGet(void) {
    uint8_t c;

#ifdef FLOWCONTROL
    rxBufferElements--;
    if ((flow == 0) && (rxBufferElements <= FLOWMARK)) {
        while (sendThisNext != 0);
        sendThisNext = XON;
        flow = 1;
        if (shouldStartTransmission) {
            shouldStartTransmission = 0;
            SERIALB |= (1 << SERIALUDRIE);
            SERIALA |= (1 << SERIALUDRE); // Trigger Interrupt
        }
    }
#endif

    if (rxRead != rxWrite) {
        c = rxBuffer[rxRead];
        rxBuffer[rxRead] = 0;
        if (rxRead < (RX_BUFFER_SIZE - 1)) {
            rxRead++;
        } else {
            rxRead = 0;
        }
        return c;
    } else {
        return 0;
    }
}

uint8_t serialRxBufferFull(void) {
    return (((rxWrite + 1) == rxRead) || ((rxRead == 0) && ((rxWrite + 1) == RX_BUFFER_SIZE)));
}

uint8_t serialRxBufferEmpty(void) {
    if (rxRead != rxWrite) {
        return 0;
    } else {
        return 1;
    }
}

// ----------------------
// |    Transmission    |
// ----------------------

void serialWrite(uint8_t data) {
#ifdef SERIALINJECTCR
    if (data == '\n') {
        serialWrite('\r');
    }
#endif
    while (serialTxBufferFull());

    txBuffer[txWrite] = data;
    if (txWrite < (TX_BUFFER_SIZE - 1)) {
        txWrite++;
    } else {
        txWrite = 0;
    }
    if (shouldStartTransmission) {
        shouldStartTransmission = 0;
        SERIALB |= (1 << SERIALUDRIE); // Enable Interrupt
        SERIALA |= (1 << SERIALUDRE); // Trigger Interrupt
    }
}

void serialWriteString(const char *data) {
    if (data == 0) {
        serialWriteString("NULL");
    } else {
        while (*data != '\0') {
            serialWrite(*data++);
        }
    }
}

uint8_t serialTxBufferFull(void) {
    return (((txWrite + 1) == txRead) || ((txRead == 0) && ((txWrite + 1) == TX_BUFFER_SIZE)));
}

uint8_t serialTxBufferEmpty(void) {
    if (txRead != txWrite) {
        return 0;
    } else {
        return 1;
    }
}
/** @} */
