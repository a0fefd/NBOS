# AS       = i686-elf-as
AS 		 = nasm
LD       = i686-elf-ld
GCC      = i686-elf-gcc
OBJCOPY  = i686-elf-objcopy

CFLAGS   ?= -O2 -g
CFLAGS   := $(CFLAGS) -Wall -Wextra -Wno-unused-variable -ffreestanding -std=gnu11

SOURCE_PATH       	= src/
BUILD_PATH        	= build/
KERNEL_SOURCE_PATH	= kernel/
LIBC_SOURCE_PATH	= libc/
BOOT_SOURCE_PATH	= boot/
ISO_PATH        	= $(BUILD_PATH)iso/

BOOT_BIN  		= $(BUILD_PATH)bin/boot.bin
BOOT2_BIN  		= $(BUILD_PATH)bin/boot2.bin
LOADER_BIN 		= $(BUILD_PATH)loader.bin
KERNEL_BIN 		= $(BUILD_PATH)bin/kernel.bin
KERNEL_ELF		= $(BUILD_PATH)kernel.elf
FLOPPY     		= $(BUILD_PATH)floppy.iso
EXPORT_DRIVE 	= floppydrive_NBOS

# Collect all sources
KERNEL_SOURCES 		= $(wildcard $(KERNEL_SOURCE_PATH)*.c $(KERNEL_SOURCE_PATH)*/*.c)
KERNEL_ASM_SOURCES 	= $(filter-out $(KERNEL_SOURCE_PATH)kernel.s, \
					  $(wildcard $(KERNEL_SOURCE_PATH)*.s))
LIBC_SOURCES   		= $(wildcard $(LIBC_SOURCE_PATH)*.c $(LIBC_SOURCE_PATH)*/*.c)
BOOT_SOURCES		= $(BOOT_SOURCE_PATH)bootsect.s

KERNEL_OBJECTS 	   ?= $(patsubst $(KERNEL_SOURCE_PATH)%.c,$(BUILD_PATH)%.o,$(KERNEL_SOURCES))
BASH_OBJECTS 		= $(patsubst $(BASH_SOURCE_PATH)%.c,$(BUILD_PATH)%.o,$(BASH_SOURCES))
KERNEL_ASM_OBJECTS 	= $(patsubst $(KERNEL_SOURCE_PATH)%.s,$(BUILD_PATH)%.o,$(KERNEL_ASM_SOURCES))
KERNEL_ALL_OBJECTS := $(KERNEL_ASM_OBJECTS) $(KERNEL_OBJECTS) $(BASH_OBJECTS) 

LIBC_OBJECTS   	= $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(LIBC_SOURCES:.c=.o))
BOOT_OBJECTS   	= $(BUILD_PATH)boot.o
OBJECTFILES    	= $(BOOT_OBJECTS) $(LIBC_OBJECTS) $(KERNEL_OBJECTS) $(BASH_OBJECTS)

KERNEL_LINKER 	= $(SOURCE_PATH)linker_kernel.ld

all: clean compilekernel buildboot makefloppy finished
	make clean

# Compilation
compilekernel:
	mkdir -p $(BUILD_PATH) $(BUILD_PATH)stdio $(BUILD_PATH)bash $(BUILD_PATH)bin

	# Compile libc source
	$(foreach src,$(LIBC_SOURCES), \
		$(GCC) -c $(src) -o $(patsubst $(LIBC_SOURCE_PATH)%,$(BUILD_PATH)%,$(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	# Compile c source
	$(foreach src,$(KERNEL_SOURCES), \
		$(GCC) -c $(src) -o $(patsubst $(KERNEL_SOURCE_PATH)%,$(BUILD_PATH)%,$(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	$(foreach src,$(BASH_SOURCES), \
		$(GCC) -c $(src) -o $(patsubst $(BASH_SOURCE_PATH)%,$(BUILD_PATH)%,$(src:.c=.o)) -std=gnu11 -ffreestanding $(CFLAGS);)

	$(AS) -f elf32 $(KERNEL_SOURCE_PATH)kernel.s -o $(BUILD_PATH)kernel.o
	$(AS) -f elf32 $(KERNEL_SOURCE_PATH)idt_s.s -o $(BUILD_PATH)idt_s.o
# 	$(AS) -f elf32 $(KERNEL_SOURCE_PATH)isr.s -o $(BUILD_PATH)isr.o

	$(LD) -T $(KERNEL_LINKER) -m elf_i386 -o $(BUILD_PATH)kernel.elf $(BUILD_PATH)kernel.o $(LIBC_OBJECTS) $(KERNEL_ALL_OBJECTS) 
	$(OBJCOPY) -O binary $(BUILD_PATH)kernel.elf $(KERNEL_BIN)

# Assembly
buildboot:
	$(AS) $(BOOT_SOURCE_PATH)bootsect.s -f bin -o $(BOOT_BIN)
	$(AS) $(BOOT_SOURCE_PATH)bootstage2.s -f bin -o $(BOOT2_BIN)

makefloppy:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -F 12 -n "NBOS" $(FLOPPY)
	dd if=$(BUILD_PATH)bin/boot.bin of=$(FLOPPY) conv=notrunc

	mcopy -i $(FLOPPY) $(BUILD_PATH)bin/boot2.bin "::stage2.bin"
	mcopy -i $(FLOPPY) $(BUILD_PATH)bin/kernel.bin "::kernel.bin"
	truncate -s 1440k $(FLOPPY)

finished:
	cp $(FLOPPY) $(EXPORT_DRIVE)

# Run
run:
# 	qemu-system-i386 -drive format=raw,file=$(EXPORT_DRIVE) -no-reboot -serial stdio -display gtk,zoom-to-fit=on
	qemu-system-i386 -drive format=raw,file=$(EXPORT_DRIVE) -no-reboot -serial stdio

# Cleanup
clean: cleanbinaries
	rm -rf $(BUILD_PATH)* *.iso

cleanobjects:
	find -maxdepth 16 -name "*.o" | xargs rm -f 

cleanbinaries: cleanobjects
	find -maxdepth 16 -name "*.elf" | xargs rm -f
	find -maxdepth 16 -name "*.bin" | xargs rm -f

cleanolds:
	find -maxdepth 16 -name "*.old" | xargs rm -rf