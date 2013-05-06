![Logo][logo]

[xyControl][gh] is a Quadrocopter Flight Controller based on Atmels Atmega2560 microcontroller.
It features 512KB SRAM on-board, using the external memory interface of this processor.
Also included is a switched power supply as well as a USB connection to communicate with and program the target.
All I/O pins, including 3 additional UARTs, SPI, I2C (TWI) and 16 ADC Channels, are accessible via standard 2.54mm connectors.
The Board can be powered from an external stable 5V supply, USB or 7V or more, via the on-board switched power supply. All voltage sources can be selected via jumpers.

[![Photo 1][xy1s]][xy1]
[![Photo 2][xy2s]][xy2]
[![Screenshot][sss]][ss]

## Flight Control Software Flow

Three tasks are controlling the Quadrocopter Orientation in Space.

+ The [Orientation Task][orient] reads the Gyroscope and Accelerometer and calculates the current Roll and Pitch angles. They are stored in the global struct "orientation".
+ The [PID Task][pid] is then feeding these angles into two PID controllers. Their output is then used by...
+ The [Set Task][set], which calculates the motor speeds and gives them to...
+ The [motor task][motor], which sends the new values via TWI to the motor controllers.

# Supported Hardware

+ Gyroscope L3GD20, code based on the [Adafruit Example][adafruit].
+ Accelerometer and Magnetometer LSM303DLHC, code based on the [Pololu Example][pololucode].
+ I got both of these Sensors on the [MinIMU-9 v2][pololu].
+ Brushless Motor Driver [BL-Ctrl V1.2][ctrl] with eg. the [Robbe Roxxy Outrunner 2824-34][mot] Brushless Motor.
+ BTM-222 Bluetooth UART Bridge ([PCB][bt])

## External Memory ([xmem.h][xmem])

The external memory consists of a 512Kx8 SRAM, bank-switched onto the 16bit avr address space.
This gives us 8 memory banks, consisting of 56KB. All memory from 0x0000 to 0x21FF is the AVRs internal memory. The memory banks are switched into 0x2200 to 0xFFFF.
This gives us 8 banks with 56KB each, resulting in 448KB external RAM.

The data and bss memory sections, as well as the Stack are located in the internal RAM. The external RAM is used only for dynamically allocated memory.

## Orientation Calculation ([orientation.h][orient])

Calculates the current angles of the platform, using Gyroscope and Accelerometer Data with a Kalman Filter. It is using this slightly modified [Kalman Filter Implementation][kalman] by Linus Helgesson.

# PC and Android Tools

You can find some PC Software in the [tools][tools] directory. Each one should be accompanied by it's own Readme file.

## UART-Flight Status Packet Format

    printf("t%.2f %.2f %.2f\n", kp, ki, kd);
    printf("u%.2f %.2f\n", pid_output[1], pid_output[0]); // Pitch, Roll
    printf("v%i %i %i %i\n", motorSpeed[0], ..., motorSpeed[3]);
    printf("w%.2f\n", orientation.pitch);
    printf("x%.2f\n", orientation.roll);
    printf("y%.2f\n", orientation.yaw);
    printf("z%.2f\n", getVoltage());

# Software used

 + [Peter Fleurys TWI Library][fleury]

# License

Peter Fleurys TWI Library (twi.c & twi.h) is released under the [GNU GPL license][gpl].

Everything else is released under a BSD-Style license. See the [accompanying COPYING file][bsd].

 [sss]: http://www.xythobuz.de/img/xyCopterOsci_small.png
 [ss]: http://www.xythobuz.de/img/xyCopterOsci.png
 [xy1s]: http://www.xythobuz.de/img/xycontrol1_small.jpg
 [xy1]: http://www.xythobuz.de/img/xycontrol1.jpg
 [xy2s]: http://www.xythobuz.de/img/xycontrol2_small.jpg
 [xy2]: http://www.xythobuz.de/img/xycontrol2.jpg
 [gh]: http://www.xythobuz.de/xycontrol/
 [logo]: http://www.xythobuz.de/xycontrol/logo.png
 [adafruit]: https://github.com/adafruit/Adafruit_L3GD20
 [pololucode]: https://github.com/pololu/LSM303
 [pololu]: http://www.pololu.com/catalog/product/1268
 [ctrl]: http://www.mikrokopter.de/ucwiki/en/BL-Ctrl_V1.2
 [mot]: http://www.conrad.de/ce/de/product/231867
 [bt]: http://xythobuz.de/bluetooth.html
 [kalman]: http://www.linushelgesson.se/2012/04/pitch-and-roll-estimating-kalman-filter-for-stabilizing-quadrocopters/
 [fleury]: http://homepage.hispeed.ch/peterfleury/avr-software.html
 [gpl]: http://www.gnu.org/licenses/gpl.html
 [bsd]: https://github.com/xythobuz/xyControl/blob/master/COPYING
 [xmem]: https://github.com/xythobuz/xyControl/blob/master/include/xmem.h
 [orient]: https://github.com/xythobuz/xyControl/blob/master/include/orientation.h
 [tools]: https://github.com/xythobuz/xyControl/tree/master/tools
 [pid]: https://github.com/xythobuz/xyControl/blob/master/include/pid.h
 [set]: https://github.com/xythobuz/xyControl/blob/master/include/set.h
 [motor]: https://github.com/xythobuz/xyControl/blob/master/include/motor.h