# xyControl

xyControl is a dev-kit for Atmels Atmega2560 microcontroller.
It features 64KB SRAM on-board, using the external memory interface of this processor.
Also included is a switched power supply as well as a USB connection to communicate with and program the target.
Additionally, a SD Card slot is provided.
All other I/O pins, including 3 additional UARTs, SPI, I2C (TWI) and 16 ADC Channels, are accessible via standard 2.54mm connectors.
The Board can be powered from an external stable 5V supply, USB or 7V or more, via the on-board switched power supply. All voltage sources can be selected via jumpers.

## External Memory

The external memory consists of a 512Kx8 SRAM, bank-switched onto the 16bit avr address space.
This gives us 8 memory banks, consisting of 56KB. All memory from 0x0000 to 0x21FF is the AVRs internal memory. The memory banks are switched into 0x2200 to 0xFFFF.
This gives us 8 banks with 56KB each, resulting in 448KB external RAM.
