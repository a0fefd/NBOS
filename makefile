AS       = i686-elf-as
LD       = i686-elf-ld
GCC      = i686-elf-gcc

CFLAGS   ?= -O2 -g
CFLAGS   := $(CFLAGS) -Wall -Wextra -ffreestanding -std=gnu11

SOURCE_PATH        = src/
BUILD_PATH         = build/
KERNEL_SOURCE_PATH = kernel/
LIBC_SOURCE_PATH   = libc/
BOOT_SOURCE_PATH   = boot/
ISO_PATH           = $(BUILD_PATH)iso/

BOOT_BIN   = $(BUILD_PATH)boot.bin
LOADER_BIN = $(BUILD_PATH)loader.bin
KERNEL_ELF = $(BUILD_PATH)kernel.elf
KERNEL_BIN = $(BUILD_PATH)kernel.bin
BOOT_IMG   = $(BUILD_PATH)boot.img

# Collect all sources
KERNEL_SOURCES 	= $(wildcard $(KERNEL_SOURCE_PATH)*.c)
LIBC_SOURCES   	= $(wildcard $(LIBC_SOURCE_PATH)*.c $(LIBC_SOURCE_PATH)*/*.c)
BOOT_SOURCES	= $(BOOT_SOURCE_PATH)boot.s $(BOOT_SOURCE_PATH)loader.s

KERNEL_OBJECTS 	= $(patsubst $(KERNEL_SOURCE_PATH)%.c,$(BUILD_PATH)%.o,$(KERNEL_SOURCES))
LIBC_OBJECTS   	= $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(LIBC_SOURCES:.c=.o))
BOOT_OBJECTS   	= $(BUILD_PATH)boot.o $(BUILD_PATH)loader.o
OBJECTFILES    	= $(BOOT_OBJECTS) $(LIBC_OBJECTS) $(KERNEL_OBJECTS)

KERNEL_LINKER 	= $(SOURCE_PATH)linker_kernel.ld

GRUBFLAGS ?= --directory=/usr/lib/grub/i386-pc --compress=gz --fonts=ascii --locales=en_AU

all: clean compilec buildboot run

# Compilation
compilec:
	mkdir -p $(BUILD_PATH) $(BUILD_PATH)stdio
	mkdir -p $(BUILD_PATH) $(BUILD_PATH)bin
	
	# Compile libc source
	$(foreach src,$(LIBC_SOURCES), \
		$(GCC) -c $(src) -o $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	# Compile c source
	$(foreach src,$(KERNEL_SOURCES), \
		$(GCC) -c $(src) -o $(BUILD_PATH)$(notdir $(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	$(AS) $(BOOT_SOURCE_PATH)test.s -o $(BUILD_PATH)test.o
	$(GCC) -T $(SOURCE_PATH)linker_kernel.ld -o $(BUILD_PATH)bin/kernel.bin -ffreestanding -O2 -nostdlib $(BUILD_PATH)test.o $(LIBC_OBJECTS) $(KERNEL_OBJECTS)

# Assembly
buildboot:

	# Assemble boot
	$(AS) $(BOOT_SOURCE_PATH)boot.s -o $(BUILD_PATH)boot.o
	$(AS) $(BOOT_SOURCE_PATH)loader.s -o $(BUILD_PATH)loader.o

# 	$(LD) -T $(SOURCE_PATH)linker_bootloader.ld --oformat binary $(BUILD_PATH)boot.o -o boot.bin
	$(LD) -T $(SOURCE_PATH)linker_bootloader.ld $(BOOT_OBJECTS) -o $(BUILD_PATH)bin/boot.bin


# Run
run:
# 	qemu-system-i386 -boot a -drive format=raw,file=$< -serial stdio
	qemu-system-i386 -drive format=raw,file=boot.bin

# Cleanup
clean:
	rm -rf $(BUILD_PATH)* *.iso

cleanobjects:
	rm -f $(BUILD_PATH)*.o $(BUILD_PATH)*/*.o
