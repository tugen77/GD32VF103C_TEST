BOARD = GD32VF103C_START

CC=riscv32-unknown-elf-gcc
AR=riscv32-unknown-elf-ar

###########################################

vpath %.S RISSC/Device
vpath %.c RISCV/gcc RISCV/drivers RISCV/stubs \
			GD32VF103_StdPeriph_Driver \
			GD32VF103_StdPeriph_Driver/Source
#			FreeRTOS/source FreeRTOS/source/port

CFLAGS  = -march=rv32imac -mabi=ilp32 -msmall-data-limit=8
CFLAGS += -Wall -fdata-sections -ffunction-sections -fshort-wchar \
		  -fstack-usage -g -O2
CFLAGS += -D$(BOARD) -DUSE_STDPERIPH_DRIVER 
#CFLAGS += -ffreestanding -nostdlib
CFLAGS += -I. -IRISCV/gcc -IRISCV/stubs -IRISCV/drivers \
		  -IGD32VF103_StdPeriph_Driver \
		  -IGD32VF103_StdPeriph_Driver/Include \
		  -IFreeRTOS/include -IFreeRTOS/include/port \
		  -I../project

#SRCS += entry.S
#SRCS = handlers.c init.c
SRCSC  = system_gd32vf103.c init.c handlers.c sbrk.c n200_func.c
SRCSC += gd32vf103_adc.c gd32vf103_bkp.c gd32vf103_can.c gd32vf103_crc.c \
		gd32vf103_dac.c gd32vf103_dbg.c gd32vf103_dma.c gd32vf103_eclic.c \
		gd32vf103_exmc.c gd32vf103_exti.c gd32vf103_fmc.c  \
		gd32vf103_fwdgt.c gd32vf103_gpio.c gd32vf103_i2c.c \
		gd32vf103_pmu.c gd32vf103_rcu.c gd32vf103_rtc.c \
		gd32vf103_spi.c gd32vf103_timer.c gd32vf103_usart.c \
		gd32vf103_wwdgt.c
SRCSA += RISCV/gcc/entry.S RISCV/gcc/start.S
#SRCS += croutine.c event_groups.c list.c queue.c stream_buffer.c \
		tasks.c timers.c port.c heap_2.c 

OBJSC = $(SRCSC:.c=.o)
OBJSA = $(SRCSA:.S=.o)

.PHONY: libgd32vf103.a

all: libgd32vf103.a

%.o : %.c
	@echo [CC] $@
	@$(CC) -c $(CFLAGS) $< -o $@
#	$(CC) $(CFLAGS) -c -o $@ $^

%.o : %.S 
	@echo [AS] $@
	@$(CC) $(CFLAGS) -c -o $@ $^
#	$(CC) $(CFLAGS) -c -o $@ $^

libgd32vf103.a: $(OBJSA) $(OBJSC)
	@echo [AR] $@
	@$(AR) -r $@ $(OBJSA) $(OBJSC)
#	$(AR) -r $@ $(OBJSA) $(OBJSC)

clean:
	rm -f $(OBJSC) $(OBJSA) *.su libgd32vf103.a

