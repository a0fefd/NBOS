CFLAGS?=-O2 -g
CFLAGS:=$(CFLAGS) -Wall -Wextra

SOURCE_PATH				=src/
BUILD_PATH				=build/
KERNEL_SOURCE_PATH		=kernel/
LIBC_SOURCE_PATH		=libc/
BOOTLOADER_SOURCE_PATH	=bootloader/
ISO_PATH				=$(BUILD_PATH)iso/

# Automatically collect all kernel source files
KERNEL_SOURCES=$(wildcard $(KERNEL_SOURCE_PATH)*.c)
KERNEL_OBJECTS=$(patsubst $(KERNEL_SOURCE_PATH)%.c,$(BUILD_PATH)%.o,$(KERNEL_SOURCES))

# Automatically collect all libc source files
LIBC_SOURCES=$(wildcard $(LIBC_SOURCE_PATH)*.c $(LIBC_SOURCE_PATH)*/*.c)
LIBC_OBJECTS=$(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(LIBC_SOURCES:.c=.o))

BOOT_OBJECT=$(BUILD_PATH)boot.o
OBJECTFILES=$(BOOT_OBJECT) $(LIBC_OBJECTS) $(KERNEL_OBJECTS)

GRUBFLAGS?=--directory=/usr/lib/grub/i386-pc --compress=gz --fonts=ascii --locales=en_AU

main:
	make clean			# Clean up build files
	make buildall 		# Build kernel
	make buildiso		# Build cdrom
	make cleanobjects	# Clean object files
	clear
	make runiso			# Load up the cdrom into QEMU i368 emulator
	clear

buildall: $(BOOTLOADER_SOURCE_PATH)boot.s $(KERNEL_SOURCE_PATH)kernel.c $(SOURCE_PATH)linker.ld
	mkdir -p $(BUILD_PATH) $(BUILD_PATH)/stdio

	# Assemble boot.s 
	i686-elf-as $(BOOTLOADER_SOURCE_PATH)boot.s -o $(BUILD_PATH)boot.o

	# Compile libc source
	$(foreach src,$(LIBC_SOURCES), \
		i686-elf-gcc -c $(src) -o $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	# Compile c source
	$(foreach src,$(KERNEL_SOURCES), \
		i686-elf-gcc -c $(src) -o $(BUILD_PATH)$(notdir $(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	# Link program into os.bin
	i686-elf-gcc -T $(SOURCE_PATH)linker.ld -o $(BUILD_PATH)os.bin -ffreestanding -O2 -nostdlib $(OBJECTFILES)

buildiso: cleanobjects
	mkdir -p $(ISO_PATH)

	mkdir -p $(ISO_PATH)boot/grub
	cp $(BUILD_PATH)os.bin $(ISO_PATH)boot/os.bin
	cp $(SOURCE_PATH)grub.cfg $(ISO_PATH)boot/grub/grub.cfg
	grub-mkrescue $(GRUBFLAGS) -o os.iso $(ISO_PATH)

run: $(BUILD_PATH)os.bin
	qemu-system-i386 -kernel $(BUILD_PATH)os.bin

runiso:
	qemu-system-i386 -cdrom os.iso

clean: cleanobjects
	rm -fr $(BUILD_PATH)*

cleanobjects:
	rm -f $(BUILD_PATH)*.o
	rm -f $(BUILD_PATH)*/*.o
	rm -f $(BUILD_PATH)*/*/*.o
