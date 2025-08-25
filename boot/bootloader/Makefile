.PHONY: $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_LIB_LIBCTZ)

ASM_BIN := $(DIR_BIN_BOOT)/asm_bootloader.o
C_BIN := $(DIR_BIN_BOOT)/c_bootloader.o
C_DEP := $(DIR_BIN_BOOT)/c_bootloader.d

$(BIN_BOOTLOADER): $(ASM_BIN) $(C_BIN) $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_LIB_LIBCTZ)
	$(LD) -T linker.ld -o $(BIN_BOOTLOADER) $^

$(ASM_BIN): bootloader.asm $(DIR_BIN_BOOT)
	$(ASM) $< -f elf64 -o $@

$(C_BIN): bootloader.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_BOOT):
	mkdir -p $(DIR_BIN_BOOT)

$(BIN_DRIVER_PRINT_VGA):
	$(MAKE) $(BIN_DRIVER_PRINT_VGA) -C $(DIR_DRIVERS)

$(BIN_DRIVER_ATA_PIO):
	$(MAKE) $(BIN_DRIVER_ATA_PIO) -C $(DIR_DRIVERS)

$(BIN_LIB_LIBCTZ):
	$(MAKE) $(BIN_LIB_LIBCTZ) -C $(DIR_UTILS)

-include $(C_DEP)
