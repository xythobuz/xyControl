# xyControl

xyControl is a dev-kit for Atmels Atmega2560 microcontroller.
It features 64KB SRAM on-board, using the external memory interface of this processor.
Also included is a switched power supply as well as a USB connection to communicate with and program the target.
Additionally, a SD Card slot is provided.
All other I/O pins, including 3 additional UARTs, SPI, I2C (TWI) and 16 ADC Channels, are accessible via standard 2.54mm connectors.
The Board can be powered from an external stable 5V supply, USB or 7V or more, via the on-board switched power supply. All voltage sources can be selected via jumpers.

# Examples

You can compile each example into it's own hex, using "make all". To compile just one example, call make with the examples filename, ending in .hex instead of .c. So, to create minimal.hex from minimal.c and the library, call "make minimal.hex".

## External Memory

The external memory consists of a 512Kx8 SRAM, bank-switched onto the 16bit avr address space.
This gives us 8 memory banks, consisting of 56KB. All memory from 0x0000 to 0x21FF is the AVRs internal memory. The memory banks are switched into 0x2200 to 0xFFFF.
This gives us 8 banks with 56KB each, resulting in 448KB external RAM.

The data and bss memory sections, as well as the Stack are located in the internal RAM. The external RAM is used only for dynamically allocated memory.

## Known Problems

In the current PCB layout, the SD-Card holder is rotated 180 degrees. This prevents an SD-Card from being inserted! As it is not a needed feature, code support will not be compiled.
