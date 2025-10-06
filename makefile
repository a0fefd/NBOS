# AS       = i686-elf-as
AS 		 = nasm
LD       = i686-elf-ld
GCC      = i686-elf-gcc

CFLAGS   ?= -O2 -g
CFLAGS   := $(CFLAGS) -Wall -Wextra -ffreestanding -std=gnu11 -m32

SOURCE_PATH        = src/
BUILD_PATH         = build/
KERNEL_SOURCE_PATH = kernel/
LIBC_SOURCE_PATH   = libc/
BOOT_SOURCE_PATH   = boot/
ISO_PATH           = $(BUILD_PATH)iso/

BOOT_BIN   = $(BUILD_PATH)bin/boot.bin
BOOT2_BIN   = $(BUILD_PATH)bin/boot2.bin
LOADER_BIN = $(BUILD_PATH)loader.bin
KERNEL_BIN = $(BUILD_PATH)bin/kernel.bin
KERNEL_ELF = $(BUILD_PATH)kernel.elf
FLOPPY     = $(BUILD_PATH)floppy.iso

# Collect all sources
KERNEL_SOURCES 	= $(wildcard $(KERNEL_SOURCE_PATH)*.c)
LIBC_SOURCES   	= $(wildcard $(LIBC_SOURCE_PATH)*.c $(LIBC_SOURCE_PATH)*/*.c)
BOOT_SOURCES	= $(BOOT_SOURCE_PATH)bootsect.s

KERNEL_OBJECTS 	= $(patsubst $(KERNEL_SOURCE_PATH)%.c,$(BUILD_PATH)%.o,$(KERNEL_SOURCES))
LIBC_OBJECTS   	= $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(LIBC_SOURCES:.c=.o))
BOOT_OBJECTS   	= $(BUILD_PATH)boot.o
OBJECTFILES    	= $(BOOT_OBJECTS) $(LIBC_OBJECTS) $(KERNEL_OBJECTS)

KERNEL_LINKER 	= $(SOURCE_PATH)linker_kernel.ld

all: clean compilekernel buildboot makefloppy

# Compilation
compilekernel:
	mkdir -p $(BUILD_PATH) $(BUILD_PATH)stdio
	mkdir -p $(BUILD_PATH) $(BUILD_PATH)bin

	# Compile libc source
	$(foreach src,$(LIBC_SOURCES), \
		$(GCC) -c $(src) -o $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	# Compile c source
	$(foreach src,$(KERNEL_SOURCES), \
		$(GCC) -c $(src) -o $(BUILD_PATH)$(notdir $(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

# 	ld -T $(KERNEL_LINKER) -o $(KERNEL_ELF) $(BUILD_PATH)lkernel.o $(LIBC_OBJECTS) $(KERNEL_OBJECTS)
# 	objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# Assembly
buildboot:
	nasm $(BOOT_SOURCE_PATH)bootsect.s -f bin -o $(BOOT_BIN)
	nasm $(BOOT_SOURCE_PATH)bootstage2.s -f bin -o $(BOOT2_BIN)

makefloppy:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -F 12 -n "NBOS" $(FLOPPY)
	dd if=$(BUILD_PATH)bin/boot.bin of=$(FLOPPY) conv=notrunc

	mcopy -i $(FLOPPY) $(BUILD_PATH)bin/boot2.bin "::stage2.bin"
	truncate -s 1440k $(FLOPPY)

# Run
run:
# 	qemu-system-i386 -drive format=raw,file=$(FLOPPY) -no-reboot
	qemu-system-i386 -drive format=raw,file=$(FLOPPY)

# Cleanup
clean:
	rm -rf $(BUILD_PATH)* *.iso

cleanobjects:
	rm -f $(BUILD_PATH)*.o $(BUILD_PATH)*/*.o
