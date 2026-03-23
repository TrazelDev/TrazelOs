include drivers.mk
include commons.mk
include utils.mk
.PHONY: clean run build debug_connect connect $(BIN_MBR) $(BIN_BOOTLOADER) $(BIN_KERNEL)

# The main options:
run: build
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMG) -serial stdio
build: $(OS_IMG)
debug: build
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMG) -s -S -serial stdio
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

$(OS_IMG): $(BIN_KERNEL)
	# 1. Create a blank image large enough to hold the partition table and the partition
	dd if=/dev/zero bs=1M count=64 of=$(OS_IMG) status=none

	# 2. Create an MBR partition table and a partition (we use parted here, but fdisk or sfdisk works too)
	parted -s $(OS_IMG) mklabel msdos
	# Start at 1MiB (2048 sectors) to leave room for the bootloader/MBR, use the rest of the disk
	parted -s $(OS_IMG) mkpart primary fat32 1MiB 100%
	parted -s $(OS_IMG) set 1 boot on

	# 3. Format the created partition (we need to format the offset where the partition starts)
	# The easiest way to do this without root (loopback) is using mtools directly with the offset.
	# 1MiB = 1048576 bytes. We can use a loop device if running as root, or format an intermediate image
	
	# Let's use the intermediate image approach as it doesn't require root:
	# Create the FAT filesystem image separately
	dd if=/dev/zero of=$(BIN_BOOT_PARTITION_IMG) bs=1M count=62 status=none
	mkfs.fat -F 32 $(BIN_BOOT_PARTITION_IMG)

	# Populate the FAT image
	mmd -i $(BIN_BOOT_PARTITION_IMG) ::boot
	mmd -i $(BIN_BOOT_PARTITION_IMG) ::boot/limine
	mcopy -i $(BIN_BOOT_PARTITION_IMG) $(BIN_KERNEL) ::kernel.bin
	mcopy -i $(BIN_BOOT_PARTITION_IMG) boot/limine/limine.conf ::limine.conf
	mcopy -i $(BIN_BOOT_PARTITION_IMG) /mnt/limine-10.8.5/common-bios/limine-bios.sys ::boot/limine/limine-bios.sys

	# 4. Write the partition image into the full disk image at the 1MiB offset
	dd if=$(BIN_BOOT_PARTITION_IMG) of=$(OS_IMG) bs=1M seek=1 conv=notrunc status=none

	# 5. Install Limine to the MBR of the final full disk image
	limine bios-install $(OS_IMG)

# $(OS_IMG): $(BIN_MBR) $(BIN_BOOT_PARTITION_IMG)
# 	dd if=/dev/zero of=$(OS_IMG) count=512 status=none
# 	dd if=$(BIN_MBR) of=$(OS_IMG) count=1 conv=notrunc status=none
# 	dd if=$(BIN_BOOT_PARTITION_IMG) of=$(OS_IMG) seek=1 conv=notrunc status=none
# 	echo ", $(BIN_BOOT_PARTITION_SECTOR_SIZE), 1, *" | sfdisk $(OS_IMG) >/dev/null
# 	# 1 - means FAT12 filing system in the the partition
# 	# * - means bootable partition 


# $(BIN_BOOT_PARTITION_IMG): $(BIN_BOOTLOADER) $(BIN_KERNEL)
# 	# Creating the partition and filing system:
# 	dd if=/dev/zero of=$(BIN_BOOT_PARTITION_IMG) count=256 status=none
# 	mkfs.fat -F 12 -R $(BIN_BOOTLOADER_SECTOR_SIZE) $(BIN_BOOT_PARTITION_IMG)
# 	# Copying the bootloader binary into the reserved sectors:
# 	dd if=$(BIN_BOOTLOADER) of=$(BIN_BOOT_PARTITION_IMG) bs=512 skip=1 seek=1 conv=notrunc status=none
# 	dd if=$(BIN_BOOTLOADER) of=$(BIN_BOOT_PARTITION_IMG) bs=1 count=3 conv=notrunc status=none
# 	mcopy -i $(BIN_BOOT_PARTITION_IMG) $(BIN_KERNEL) ::kernel.bin

$(BIN_KERNEL):
	$(MAKE) $(BIN_KERNEL) -C $(DIR_KERNEL)
	
$(BIN_MBR):
	$(MAKE) $(BIN_MBR) -C $(DIR_MBR)

$(BIN_BOOTLOADER):
	$(MAKE) $(BIN_BOOTLOADER) -C $(DIR_BOOTLOADER)
