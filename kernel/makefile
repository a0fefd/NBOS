include ../toolchain.mk

CFLAGS = -std=gnu11 -ffreestanding -O2 -g -Wall -Wextra -Wno-unused-variable

BUILD_PATH_KRNL = ../build/kernel

KRNL_ASM_SOURCE = $(wildcard *.s)
KRNL_C_SOURCE   = $(wildcard *.c)

i686_ASM_SOURCE = $(wildcard i686/*.s)
i686_C_SOURCE   = $(wildcard i686/*.c)

.PHONY: all i686 krnl_src always

all: i686_src krnl_src

krnl_src: $(KRNL_ASM_SOURCE) $(KRNL_C_SOURCE)
	mkdir -p $(BUILD_PATH_KRNL)

	$(foreach csrc,$(KRNL_C_SOURCE), \
		$(GCC) -c $(csrc) -o $(patsubst %,$(BUILD_PATH_KRNL)/%,$(csrc:.c=.o)) $(CFLAGS); )

	$(foreach asmsrc,$(KRNL_ASM_SOURCE), \
		$(AS) -f elf32 $(asmsrc) -o $(patsubst %,$(BUILD_PATH_KRNL)/%,$(asmsrc:.s=_s.o)); )

i686_src: $(i686_ASM_SOURCE) $(i686_C_SOURCE)
	mkdir -p $(BUILD_PATH_KRNL)/i686/

	$(foreach csrc,$(i686_C_SOURCE), \
		$(GCC) -c $(csrc) -o $(patsubst i686/%,$(BUILD_PATH_KRNL)/i686/%,$(csrc:.c=.o)) $(CFLAGS); )

	$(foreach asmsrc,$(i686_ASM_SOURCE), \
		$(AS) -f elf32 $(asmsrc) -o $(patsubst i686/%,$(BUILD_PATH_KRNL)/i686/%,$(asmsrc:.s=_s.o)); )