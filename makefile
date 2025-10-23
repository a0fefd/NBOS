include toolchain.mk

SOURCE_PATH       	= src
BUILD_PATH        	= build

KERNEL_ALL_OBJECTS ?= $(filter-out $(BUILD_PATH)/kernel/kernel_s.o, $(wildcard $(BUILD_PATH)/kernel/*.o $(BUILD_PATH)/kernel/i686/*.o))
KERNEL_ALL_OBJECTS := $(KERNEL_ALL_OBJECTS) $(wildcard $(BUILD_PATH)/libc/*.o $(BUILD_PATH)/libc/stdio/*.o)

KERNEL_LINKER 	= $(SOURCE_PATH)/linker_kernel.ld

all: clean krnl bootloader floppy finished

# Compilation
krnl:
	make -C kernel/
	make -C libc/

	$(LD) -T $(KERNEL_LINKER) -nostdlib -m elf_i386 -o $(BUILD_PATH)/kernel/kernel.elf $(BUILD_PATH)/kernel/kernel_s.o $(KERNEL_ALL_OBJECTS)
	$(OBJCOPY) -O binary $(BUILD_PATH)/kernel/kernel.elf $(BUILD_PATH)/kernel/kernel.bin

bootloader:
	make -C boot/


floppy:
	dd if=/dev/zero of=$(BUILD_PATH)/floppy.iso bs=512 count=2880
	mkfs.fat -F 12 -n "NBOS" $(BUILD_PATH)/floppy.iso
	dd if=$(BUILD_PATH)/bootloader/bootsect.bin of=$(BUILD_PATH)/floppy.iso conv=notrunc

	mcopy -i $(BUILD_PATH)/floppy.iso $(BUILD_PATH)/bootloader/bootstage2.bin "::stage2.bin"
	mcopy -i $(BUILD_PATH)/floppy.iso $(BUILD_PATH)/kernel/kernel.bin "::kernel.bin"

finished:
	cp $(BUILD_PATH)/floppy.iso floppydrive_NBOS.iso

# Run
run:
# 	qemu-system-i386 -drive format=raw,file=$(EXPORT_DRIVE) -no-reboot -serial stdio -display gtk,zoom-to-fit=on
# 	qemu-system-i386 -drive format=raw,file=$(EXPORT_DRIVE)
	qemu-system-i386 -fda floppydrive_NBOS.iso -serial stdio

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