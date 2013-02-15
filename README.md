# xyControl

xyControl is a Quadrocopter Flight Controller based on Atmels Atmega2560 microcontroller.
It features 64KB SRAM on-board, using the external memory interface of this processor.
Also included is a switched power supply as well as a USB connection to communicate with and program the target.
All I/O pins, including 3 additional UARTs, SPI, I2C (TWI) and 16 ADC Channels, are accessible via standard 2.54mm connectors.
The Board can be powered from an external stable 5V supply, USB or 7V or more, via the on-board switched power supply. All voltage sources can be selected via jumpers.

## Known Problems

In the current PCB layout, the SD-Card holder is rotated 180 degrees. This prevents an SD-Card from being inserted!

# Examples

You can compile each example into it's own hex, using "make all". To compile just one example, call make with the examples filename, ending in .hex instead of .c. So, to create minimal.hex from minimal.c and the library, call "make minimal.hex".

## Old Examples

These example projects don't use the Task Scheduler.

# Supported Hardware

+ Gyroscope L3GD20, code based on the [Adafruit Example](https://github.com/adafruit/Adafruit_L3GD20).
+ Accelerometer and Magnetometer LSM303DLHC, code based on the [Pololu Example](https://github.com/pololu/LSM303).
+ I got both of these Sensors on the [MinIMU-9 v2](http://www.pololu.com/catalog/product/1268)
+ Brushless Motor Driver [BL-Ctrl V1.2](http://www.mikrokopter.de/ucwiki/en/BL-Ctrl_V1.2) with eg. the [Robbe Roxxy Outrunner 2824-34](http://www.conrad.de/ce/de/product/231867) Brushless Motor.

## External Memory (xmem.h)

The external memory consists of a 512Kx8 SRAM, bank-switched onto the 16bit avr address space.
This gives us 8 memory banks, consisting of 56KB. All memory from 0x0000 to 0x21FF is the AVRs internal memory. The memory banks are switched into 0x2200 to 0xFFFF.
This gives us 8 banks with 56KB each, resulting in 448KB external RAM.

The data and bss memory sections, as well as the Stack are located in the internal RAM. The external RAM is used only for dynamically allocated memory.

## Orientation Calculation (orientation.h)

Calculates the current angles of the platform, using Gyroscope and Accelerometer Data with a complementary filter. For more informations on the theory, see this [Slideshow](http://web.mit.edu/scolton/www/filter.pdf) (PDF!).

# PC Tools

You can find some PC Software in the 'tools' directory. Each one should be accompanied by it's own Readme file.

# Software used

 + [Peter Fleurys TWI Library](http://homepage.hispeed.ch/peterfleury/avr-software.html)

# License

Peter Fleurys TWI Library (twi.c & twi.h) is released under the [GNU GPL license](http://www.gnu.org/licenses/gpl.html).

Everything else is released under a BSD-Style license. See the accompanying COPYING file.
