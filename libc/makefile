include ../toolchain.mk

CFLAGS = -std=gnu11 -ffreestanding -O2 -g -Wall -Wextra -Wno-unused-variable

BUILD_PATH_LIBC = ../build/libc

LIBC_ASM_SOURCE = $(wildcard *.s */*.s)
LIBC_C_SOURCE   = $(wildcard *.c */*.c)

.PHONY: all libc_src always

all: libc_src

libc_src: $(LIBC_ASM_SOURCE) $(LIBC_C_SOURCE)
	mkdir -p $(BUILD_PATH_LIBC)/stdio

	$(foreach csrc,$(LIBC_C_SOURCE), \
		$(GCC) -c $(csrc) -o $(patsubst %,$(BUILD_PATH_LIBC)/%,$(csrc:.c=.o)) $(CFLAGS); )

	$(foreach asmsrc,$(LIBC_ASM_SOURCE), \
		$(AS) -f elf32 $(asmsrc) -o $(patsubst %,$(BUILD_PATH_LIBC)/%,$(asmsrc:.s=_s.o)); )
