include commons.mk
.PHONY: clean run build

# The main options:
build: $(OS_IMG)
run:
	sudo qemu-system-x86_64 $(OS_IMG)
clean:
	rm -rf bin


$(OS_IMG): $(BIN_MBR) $(BIN_BOOTLOADER)
	dd if=/dev/zero of=$(OS_IMG) count=100
	dd if=$(BIN_MBR) of=$(OS_IMG) count=1 conv=notrunc
	dd if=$(BIN_BOOTLOADER) of=$(OS_IMG) seek=1 conv=notrunc
	echo ', 5, L, *' | sfdisk $(OS_IMG)


$(BIN_MBR):
	$(MAKE) $(BIN_MBR) -C $(DIR_MBR)

$(BIN_BOOTLOADER):
	$(MAKE) $(BIN_BOOTLOADER) -C $(DIR_BOOTLOADER)
