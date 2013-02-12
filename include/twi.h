/*************************************************************************
* Title:    I2C master library using hardware TWI interface
* Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
* File:     $Id: twimaster.c,v 1.3 2005/07/02 11:14:21 Peter Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device with hardware TWI
* Usage:    API compatible with I2C Software Library i2cmaster.h
**************************************************************************/

#ifndef _twi_h
#define _twi_h

#define TWI_READ    1
#define TWI_WRITE   0

void twiInit(void);
void twiStop(void);
unsigned char twiStart(unsigned char addr); // 1 = failed to access device
unsigned char twiRepStart(unsigned char addr); // 1 = failed to access device
void twiStartWait(unsigned char addr);
unsigned char twiWrite(unsigned char data); // 1 = write failed
unsigned char twiReadAck(void);
unsigned char twiReadNak(void);
#define twiRead(ack)  ((ack) ? i2c_readAck() : i2c_readNak())

#endif
