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
EXTRAINCDIR = include
CSTANDARD = gnu99

BASEDIR = /usr/local/CrossPack-AVR/bin
YASABDIR = /Users/thomas/bin
GCC = $(BASEDIR)/avr-gcc
SIZE = $(BASEDIR)/avr-size
OBJCOPY = $(BASEDIR)/avr-objcopy
OBJDUMP = $(BASEDIR)/avr-objdump
AVRDUDE = $(BASEDIR)/avrdude
YASAB = $(YASABDIR)/yasab
DOXYGEN = /Applications/Doxygen.app/Contents/Resources/doxygen

SRC = lib/lowlevel/adc.c
SRC += lib/lowlevel/serial.c
SRC += lib/lowlevel/time.c
SRC += lib/lowlevel/twi.c
SRC += lib/lowlevel/xmem.c
SRC += lib/acc.c
SRC += lib/error.c
SRC += lib/gyro.c
SRC += lib/lcd.c
SRC += lib/mag.c
SRC += lib/remote.c

OBJ = $(SRC:.c=.o)

CARGS = -mmcu=$(MCU)
CARGS += -I$(EXTRAINCDIR)
CARGS += -Os
CARGS += -funsigned-char
CARGS += -funsigned-bitfields
CARGS += -fpack-struct
CARGS += -fshort-enums
CARGS += -Wall -pedantic -Wstrict-prototypes -Wshadow
CARGS += -Wpointer-arith -Wcast-qual -Wextra
CARGS += -Wno-write-strings -Wno-unused-parameter
CARGS += -std=$(CSTANDARD)
CARGS += -DF_CPU=$(F_CPU)
CARGS += -ffunction-sections

LINKER = -Wl,--relax
LINKER += -Wl,--defsym=__heap_start=0x802200,--defsym=__heap_end=0x80ffff
LINKER += -Wl,-gc-sections

PROGRAMMER = avrisp2
ISPPORT = usb
BOOTLOADER = /dev/tty.usbserial-AE01539L
#BOOTLOADER = /dev/tty.xyRobot-DevB

TARGETSRC = flight.c

TARGETHEX = $(TARGETSRC:.c=.hex)
TARGETFLASH = $(TARGETSRC:.c=.flash)
TARGETPROGRAM = $(TARGETSRC:.c=.program)

all: $(TARGETHEX)

program: $(TARGETPROGRAM)

flash: $(TARGETFLASH)

doc: runDoxygen setPDF

runDoxygen:
	$(DOXYGEN) Doxyfile

setPDF:
	make -C doc/latex/

dropbox: dropboxCopy
	make clean

dropboxCopy: $(TARGETHEX)
	cp $(TARGETHEX) ~/Dropbox/

%.o: %.c
	$(GCC) -c $< -o $@ $(CARGS)

%.elf: %.o $(OBJ)
	$(GCC) $(CARGS) $< $(OBJ) --output $@ $(LINKER) -Wl,-Map -Wl,$(@:.elf=.map)
	$(SIZE) --mcu=$(MCU) -C $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@
	$(OBJDUMP) -h -S $< > $(@:.hex=.lss)

%.program: %.hex
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(ISPPORT) -e -U $<

%.flash: %.hex
	$(YASAB) $(BOOTLOADER) $< q

clean:
	$(RM) $(OBJ)
	$(RM) *.o
	$(RM) *.elf
	$(RM) *.hex
	$(RM) *.lss
	$(RM) *.map
