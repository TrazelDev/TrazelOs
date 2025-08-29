include drivers.mk
include commons.mk
include utils.mk
.PHONY: clean run build $(BIN_MBR) $(BIN_BOOTLOADER)

# The main options:
build: $(OS_IMG)
run:
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMG)
clean:
	rm -rf bin



# caclualted at run time:
BIN_BOOTLOADER_SECTOR_SIZE = $(shell stat --format='%s' $(BIN_BOOTLOADER) | awk '{print int(($$1+511)/512)}')
BIN_BOOT_PARTION_SECTOR_SIZE = $(shell stat --format='%s' $(BIN_BOOT_PARTION_IMG) | awk '{print int(($$1+511)/512)}')

$(OS_IMG): $(BIN_MBR) $(BIN_BOOT_PARTION_IMG)
	dd if=/dev/zero of=$(OS_IMG) count=512 status=none
	dd if=$(BIN_MBR) of=$(OS_IMG) count=1 conv=notrunc status=none
	dd if=$(BIN_BOOT_PARTION_IMG) of=$(OS_IMG) seek=1 conv=notrunc status=none
	echo ", $(BIN_BOOT_PARTION_SECTOR_SIZE), 1, *" | sfdisk $(OS_IMG) >/dev/null
	# 1 - means FAT12 filing system in the the partion
	# * - means bootable partion 

$(BIN_BOOT_PARTION_IMG): $(BIN_BOOTLOADER)
	# Creating the partion and filing system:
	dd if=/dev/zero of=$(BIN_BOOT_PARTION_IMG) count=128 status=none
	mkfs.fat -F 12 -R $(BIN_BOOTLOADER_SECTOR_SIZE) $(BIN_BOOT_PARTION_IMG)
	# Coppying the bootloader binnary into the reserved sectors:
	dd if=$(BIN_BOOTLOADER) of=$(BIN_BOOT_PARTION_IMG) bs=512 skip=1 seek=1 conv=notrunc status=none
	dd if=$(BIN_BOOTLOADER) of=$(BIN_BOOT_PARTION_IMG) bs=1 count=3 conv=notrunc status=none

$(BIN_MBR):
	$(MAKE) $(BIN_MBR) -C $(DIR_MBR)

$(BIN_BOOTLOADER):
	$(MAKE) $(BIN_BOOTLOADER) -C $(DIR_BOOTLOADER)
