include drivers.mk
include commons.mk
include utils.mk
.PHONY: clean run build debug_connect connect $(BIN_MBR) $(BIN_BOOTLOADER) $(BIN_KERNEL)

# The main options:
run: build
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMG)
build: $(OS_IMG)
debug: build
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMG) -s -S &
connect: debug_connect
debug_connect:
	gdb bin/kernel.bin
rebuild:
	$(MAKE) clean
	$(MAKE) build
clean:
	rm -rf bin



# calculated at run time:
BIN_BOOTLOADER_SECTOR_SIZE = $(shell stat --format='%s' $(BIN_BOOTLOADER) | awk '{print int(($$1+511)/512)}')
BIN_BOOT_PARTITION_SECTOR_SIZE = $(shell stat --format='%s' $(BIN_BOOT_PARTITION_IMG) | awk '{print int(($$1+511)/512)}')

$(OS_IMG): $(BIN_MBR) $(BIN_BOOT_PARTITION_IMG)
	dd if=/dev/zero of=$(OS_IMG) count=512 status=none
	dd if=$(BIN_MBR) of=$(OS_IMG) count=1 conv=notrunc status=none
	dd if=$(BIN_BOOT_PARTITION_IMG) of=$(OS_IMG) seek=1 conv=notrunc status=none
	echo ", $(BIN_BOOT_PARTITION_SECTOR_SIZE), 1, *" | sfdisk $(OS_IMG) >/dev/null
	# 1 - means FAT12 filing system in the the partition
	# * - means bootable partition 

$(BIN_BOOT_PARTITION_IMG): $(BIN_BOOTLOADER) $(BIN_KERNEL)
	# Creating the partition and filing system:
	dd if=/dev/zero of=$(BIN_BOOT_PARTITION_IMG) count=128 status=none
	mkfs.fat -F 12 -R $(BIN_BOOTLOADER_SECTOR_SIZE) $(BIN_BOOT_PARTITION_IMG)
	# Copying the bootloader binary into the reserved sectors:
	dd if=$(BIN_BOOTLOADER) of=$(BIN_BOOT_PARTITION_IMG) bs=512 skip=1 seek=1 conv=notrunc status=none
	dd if=$(BIN_BOOTLOADER) of=$(BIN_BOOT_PARTITION_IMG) bs=1 count=3 conv=notrunc status=none
	mcopy -i $(BIN_BOOT_PARTITION_IMG) $(BIN_KERNEL) ::kernel.bin

$(BIN_KERNEL):
	$(MAKE) $(BIN_KERNEL) -C $(DIR_KERNEL)
	
$(BIN_MBR):
	$(MAKE) $(BIN_MBR) -C $(DIR_MBR)

$(BIN_BOOTLOADER):
	$(MAKE) $(BIN_BOOTLOADER) -C $(DIR_BOOTLOADER)
