CROSS ?= $(HOME)/zephyr-sdk-1.0.1/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-
CC     = $(CROSS)gcc
OBJCOPY= $(CROSS)objcopy
SIZE   = $(CROSS)size

CFLAGS = -mcpu=cortex-m0plus -mthumb -Os -g -Wall -Wextra -ffreestanding \
         -ffunction-sections -fdata-sections
LDFLAGS= -T linker.ld -nostdlib -nostartfiles -Wl,--gc-sections -Wl,-Map=build/rgb.map

all: build/rgb.elf build/rgb.bin
	$(SIZE) build/rgb.elf

build/rgb.elf: main.c startup.c linker.ld | build
	$(CC) $(CFLAGS) $(LDFLAGS) main.c startup.c -o $@

build/rgb.bin: build/rgb.elf
	$(OBJCOPY) -O binary $< $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean
