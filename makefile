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
OPT = s
EXTRAINCDIR = include
CSTANDARD = gnu99

SRC = main.c
SRC += lib/time.c
SRC += lib/spi.c
SRC += lib/serial.c
SRC += lib/xmem.c
SRC += lib/xycontrol.c

OBJ = $(SRC:.c=.o)

CARGS = -mmcu=$(MCU)
CARGS += -I$(EXTRAINCDIR)
CARGS += -O$(OPT)
CARGS += -funsigned-char
CARGS += -funsigned-bitfields
CARGS += -fpack-struct
CARGS += -fshort-enums
CARGS += -Wall -Wstrict-prototypes
CARGS += -std=$(CSTANDARD)
CARGS += -DF_CPU=$(F_CPU)

LINKER = -Wl,--relax
LINKER += -Wl,-Map -Wl,test.map
LINKER += -Wl,--defsym=__heap_start=0x802200,--defsym=__heap_end=0x80ffff

all: test.hex

program: test.hex
	yasab /dev/tty.usbserial-A100QOUE test.hex q

flash: test.hex
	avrdude -p $(MCU) -c stk500v2 -P /dev/tty.usbmodem641 -e -U test.hex

%.o: %.c
	avr-gcc -c $< -o $@ $(CARGS)

test.elf: $(OBJ)
	avr-gcc $(CARGS) $(OBJ) --output test.elf $(LINKER)
	avr-size --mcu=$(MCU) -C test.elf

test.hex: test.elf
	avr-objcopy -O ihex test.elf test.hex
	avr-objdump -h -S test.elf > test.lss

clean:
	$(RM) $(OBJ)
	$(RM) *.elf
	$(RM) *.hex
	$(RM) *.lss
	$(RM) *.map
