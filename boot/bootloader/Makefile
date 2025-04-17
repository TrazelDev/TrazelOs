DIR_BIN_BOOT := $(BIN_DIR)/boot


$(BIN_BOOTLOADER):
	mkdir -p $(DIR_BIN_BOOT)
	$(ASM) bootloader.asm -f elf64 -o $(DIR_BIN_BOOT)/asm_bootloader.o
	$(CC) $(CC_FLAGS) -c bootloader.c -o $(DIR_BIN_BOOT)/c_bootloader.o
	$(LD) -T linker.ld -o $(BIN_BOOTLOADER) $(DIR_BIN_BOOT)/asm_bootloader.o $(DIR_BIN_BOOT)/c_bootloader.o
