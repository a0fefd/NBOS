include ../toolchain.mk

BUILD_PATH_BOOT = ../build/bootloader

BOOT_ASM_SOURCE = $(wildcard *.s */*.s)

.PHONY: all boot_src always

all: boot_src

boot_src: $(BOOT_ASM_SOURCE)
	mkdir -p $(BUILD_PATH_BOOT)

	$(foreach asmsrc,$(BOOT_ASM_SOURCE), \
		$(AS) -f bin $(asmsrc) -o $(patsubst %,$(BUILD_PATH_BOOT)/%,$(asmsrc:.s=.bin)); )
