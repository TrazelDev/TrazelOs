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

$(OS_IMG): $(BIN_MBR) $(BIN_BOOTLOADER)
	dd if=/dev/zero of=$(OS_IMG) count=100 status=none
	dd if=$(BIN_MBR) of=$(OS_IMG) count=1 conv=notrunc status=none
	dd if=$(BIN_BOOTLOADER) of=$(OS_IMG) seek=1 conv=notrunc status=none
	echo ", $(BIN_BOOTLOADER_SECTOR_SIZE), L, *" | sfdisk $(OS_IMG) >/dev/null \


$(BIN_MBR):
	$(MAKE) $(BIN_MBR) -C $(DIR_MBR)

$(BIN_BOOTLOADER):
	$(MAKE) $(BIN_BOOTLOADER) -C $(DIR_BOOTLOADER)
