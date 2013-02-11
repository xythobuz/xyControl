# Copyright (c) 2012, Thomas Buck <xythobuz@me.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MCU = atmega2560
F_CPU = 16000000
RM = rm -rf
EXTRAINCDIR = ../include
CSTANDARD = gnu99

SRC = ../lib/time.c
SRC += ../lib/spi.c
SRC += ../lib/serial.c
SRC += ../lib/xmem.c
SRC += ../lib/xycontrol.c
SRC += ../lib/tasks.c
SRC += ../lib/twi.c
SRC += ../lib/gyro.c
SRC += ../lib/acc.c
SRC += ../lib/mag.c
SRC += ../lib/motor.c
SRC += ../lib/adc.c
SRC += ../lib/uartMenu.c
SRC += ../lib/orientation.c

OBJ = $(SRC:.c=.o)

CARGS = -mmcu=$(MCU)
CARGS += -I$(EXTRAINCDIR)
CARGS += -Os
CARGS += -funsigned-char
CARGS += -funsigned-bitfields
CARGS += -fpack-struct
CARGS += -fshort-enums
CARGS += -Wall -Wstrict-prototypes
CARGS += -std=$(CSTANDARD)
CARGS += -DF_CPU=$(F_CPU)
CARGS += -lm -lprintf_flt

LINKER = -Wl,--relax,-u,vfprintf,-lm,-lprintf_flt
LINKER += -Wl,--defsym=__heap_start=0x802200,--defsym=__heap_end=0x80ffff

PROGRAMMER = stk500v2
ISPPORT = /dev/tty.usbmodem641
BOOTLOADER = /dev/tty.usbserial-AE01539L

all: minimal.hex hardwareTest.hex

%.o: %.c
	avr-gcc -c $< -o $@ $(CARGS)

%.elf: %.o $(OBJ)
	avr-gcc $(CARGS) $< $(OBJ) --output $@ $(LINKER) -Wl,-Map -Wl,$(@:.elf=.map)
	avr-size --mcu=$(MCU) -C $@

%.hex: %.elf
	avr-objcopy -O ihex $< $@
	avr-objdump -h -S $< > $(@:.hex=.lss)

%.program: %.hex
	avrdude -p $(MCU) -c $(PROGRAMMER) -P $(ISPPORT) -e -U $<

%.flash: %.hex
	yasab $(BOOTLOADER) $< q

clean:
	$(RM) $(OBJ)
	$(RM) *.o
	$(RM) *.elf
	$(RM) *.hex
	$(RM) *.lss
	$(RM) *.map
