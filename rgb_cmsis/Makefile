CROSS ?= $(HOME)/zephyr-sdk-1.0.1/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-
HAL   ?= $(HOME)/zephyrproject/modules/hal
TI    := $(HAL)/ti/mspm0/source
CMSIS := $(HAL)/cmsis_6/CMSIS/Core/Include

CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
SIZE    = $(CROSS)size

CFLAGS = -mcpu=cortex-m0plus -mthumb -Os -g -Wall -Wextra -ffreestanding \
         -ffunction-sections -fdata-sections \
         -D__MSPM0L1117__ -DDeviceFamily_MSPM0L111X \
         -Icompat -I$(TI) -I$(CMSIS)
LDFLAGS = -T linker.ld -nostdlib -nostartfiles -Wl,--gc-sections -Wl,-Map=build/rgb_cmsis.map

all: build/rgb_cmsis.elf build/rgb_cmsis.bin
	$(SIZE) build/rgb_cmsis.elf

build/rgb_cmsis.elf: main.c startup.c linker.ld | build
	$(CC) $(CFLAGS) $(LDFLAGS) main.c startup.c -o $@

build/rgb_cmsis.bin: build/rgb_cmsis.elf
	$(OBJCOPY) -O binary $< $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean
