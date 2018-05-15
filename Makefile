TARGET      = distance
PROGRAMMER  = usbtiny
MCU         = atmega328
F_CPU       = 16000000UL
SRCS        = $(wildcard *.c) $(wildcard util/*.c)
OBJS        = $(subst .c,.o,$(SRCS))
INC         = -I. -Iutil
GIT_VERSION = $(shell git describe --abbrev=4 --dirty --always --tags)
TOOLS_PREFIX = avr-
CC          = $(TOOLS_PREFIX)gcc
#--------------------------------
# Compiler flags
#--------------------------------
## Warnings, standards
CFLAGS      = -O2 -Wall -std=gnu99
## Hardware definitions
CFLAGS     += -DF_CPU=$(F_CPU) -mmcu=$(MCU) -DGIT_VERSION=\"$(GIT_VERSION)\"
## Use short (8-bit) data types 
CFLAGS     += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
## Splits up object files per function
CFLAGS     += -ffunction-sections -fdata-sections 
#--------------------------------
# Linker flags
#--------------------------------
LDFLAGS     = -Wl,-Map=$(TARGET).map,--cref
# Optional, but often ends up with smaller code
LDFLAGS    += -Wl,--gc-sections
## Relax shrinks code even more, but makes disassembly messy
LDFLAGS += -Wl,--relax
## for floating-point printf
# LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm
## for smaller printf
# LDFLAGS += -Wl,-u,vfprintf -lprintf_min

all: clean $(TARGET).hex $(TARGET).lst flash

flash: $(TARGET).hex 
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<

%.o: %.c
	@echo -----------------------------------------------
	@echo  Compiling $@
	@echo -----------------------------------------------
	$(CC) $(INC) $(CFLAGS) -o $@ -c $<

%.lst: %.elf
	@echo -----------------------------------------------
	@echo  Generating assembly listing
	@echo -----------------------------------------------	
	$(TOOLS_PREFIX)objdump -h -S -z $< > $@

%.hex: %.elf
	@echo -----------------------------------------------
	@echo  Generating intel hex file
	@echo -----------------------------------------------		
	$(TOOLS_PREFIX)objcopy -O ihex -R .eeprom $< $@

$(TARGET).elf: $(OBJS)
	@echo -----------------------------------------------
	@echo  Linking together an application .elf file
	@echo -----------------------------------------------
	$(CC) -mmcu=$(MCU) $(LDFLAGS) -o $@ $^
	chmod -x $@
	$(TOOLS_PREFIX)size $@

clean:
	rm -f $(TARGET).hex $(TARGET).lst $(TARGET).elf $(TARGET).map $(OBJS)

.PHONY: clean all
#.INTERMEDIATE: $(TARGET).lst $(TARGET).elf