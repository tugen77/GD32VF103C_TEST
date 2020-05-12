PROJ_NAME=GD32VF103C_TEST

# put your *.o targets here, make should handle the rest!
SRCS  = main.c 
SRCS += systick.c 
SRCS += uart.c 
SRCS += misc.c 
SRCS += led.c 
SRCS += adc.c 
SRCS += can.c 

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
BOARD = GD32VF103C_START

MSG_SIZE = File size:

# Location of the Libraries folder from the STM32F0xx Standard Peripheral Library
STD_PERIPH_LIB=Libraries

# Location of the linker scripts
LDSCRIPT_INC=$(STD_PERIPH_LIB)/RISCV/gcc

# that's it, no need to change anything below this line!

###################################################

CC=riscv32-unknown-elf-gcc
OBJCOPY=riscv32-unknown-elf-objcopy
OBJDUMP=riscv32-unknown-elf-objdump
SIZE=riscv32-unknown-elf-size

DEFS = -D$(BOARD)

CFLAGS  = -march=rv32imac -mabi=ilp32 -msmall-data-limit=8
CFLAGS += -Wall -D$(BOARD) -DUSE_STDPERIPH_DRIVER 
CFLAGS += -nostartfiles -lm -lc -lgcc
CFLAGS += -Wall -fdata-sections -ffunction-sections -fshort-wchar \
		  -fstack-usage -g -O2

LDFLAGS = -g -Og -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map,--cref $(DEFS)

###################################################

vpath %.c project
vpath %.a $(STD_PERIPH_LIB)

ROOT=$(shell pwd)

CFLAGS += -I$(STD_PERIPH_LIB) 
CFLAGS += -I$(STD_PERIPH_LIB)/RISCV/drivers 
CFLAGS += -I$(STD_PERIPH_LIB)/GD32VF103_StdPeriph_Driver
CFLAGS += -I$(STD_PERIPH_LIB)/GD32VF103_StdPeriph_Driver/Include
CFLAGS += -I$(STD_PERIPH_LIB)/FreeRTOS/include 
CFLAGS += -I$(STD_PERIPH_LIB)/FreeRTOS/include/port -Iproject

ifeq ($(CPU), GD32VF103X4)
LDFILE = GD32VF103x4.lds
else ifeq ($(CPU), GD32VF103X6)
LDFILE = GD32VF103x6.lds
else ifeq ($(CPU), GD32VF103X8)
LDFILE = GD32VF103x8.lds
else ifeq ($(CPU), GD32VF103XB)
LDFILE = GD32VF103xB.lds
else 
LDFILE = GD32VF103x8.lds
endif

ELFSIZE = $(SIZE) -A $(PROJ_NAME).elf
#SRCS += $(STD_PERIPH_LIB)/RISCV/Device/start.S
#SRCS += $(STD_PERIPH_LIB)/RISCV/Device/entry.S

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: lib proj

all: lib $(OBJS) link hex size

lib:
	$(MAKE) -C $(STD_PERIPH_LIB)

%.o: %.c
	@echo [CC] $@
	@$(CC) -c $(CFLAGS) $< -o $@
#	$(CC) -c $(CFLAGS) $< -o $@

link:
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -L$(STD_PERIPH_LIB) -lgd32vf103 -L$(LDSCRIPT_INC) -T$(LDFILE) -o $(PROJ_NAME).elf

hex:
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(OBJDUMP) -St $(PROJ_NAME).elf >$(PROJ_NAME).lst

size:
	@if [ -f $(PROJ_NAME).elf ]; then echo; echo $(MSG_SIZE); $(ELFSIZE); echo; fi

#proj: 	$(PROJ_NAME).elf

#$(PROJ_NAME).elf: $(SRCS)
#	$(CC) $(CFLAGS) $^ -o $@ -L$(STD_PERIPH_LIB) -lgd32vf103 -L$(LDSCRIPT_INC) -T$(LDFILE) $(LDFLAGS)
#	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
#	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
#	$(OBJDUMP) -St $(PROJ_NAME).elf >$(PROJ_NAME).lst
#	$(SIZE) $(PROJ_NAME).elf
	
clean:
	find ./ -name '*~' | xargs rm -f	
	rm -f *.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	rm -f $(PROJ_NAME).map
	rm -f $(PROJ_NAME).lst
	rm -f *.su

reallyclean: clean
	$(MAKE) -C $(STD_PERIPH_LIB) clean






