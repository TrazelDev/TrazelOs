ASM_BIN := $(DIR_BIN_BOOT)/asm_bootloader.o
C_BIN := $(DIR_BIN_BOOT)/c_bootloader.o

$(BIN_BOOTLOADER): $(ASM_BIN) $(C_BIN)
	$(LD) -T linker.ld -o $(BIN_BOOTLOADER) $^

$(ASM_BIN): bootloader.asm $(DIR_BIN_BOOT)
	$(ASM) $< -f elf64 -o $@

$(C_BIN): bootloader.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_BOOT):
	mkdir -p $(DIR_BIN_BOOT)
