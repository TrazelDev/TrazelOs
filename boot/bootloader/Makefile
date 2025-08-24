.PHONY: $(BIN_DRIVER_PRINT_VGA)

ASM_BIN := $(DIR_BIN_BOOT)/asm_bootloader.o
C_BIN := $(DIR_BIN_BOOT)/c_bootloader.o
HARD_DRIVE_BIN := $(DIR_BIN_BOOT)/hard_drive_driver.o
C_DEP := $(DIR_BIN_BOOT)/c_bootloader.d

$(BIN_BOOTLOADER): $(ASM_BIN) $(C_BIN) $(HARD_DRIVE_BIN) $(BIN_DRIVER_PRINT_VGA)
	$(LD) -T linker.ld -o $(BIN_BOOTLOADER) $^

$(ASM_BIN): bootloader.asm $(DIR_BIN_BOOT)
	$(ASM) $< -f elf64 -o $@

$(C_BIN): bootloader.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(HARD_DRIVE_BIN): hard_drive_driver.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_BOOT):
	mkdir -p $(DIR_BIN_BOOT)

$(BIN_DRIVER_PRINT_VGA):
	$(MAKE) $(BIN_DRIVER_PRINT_VGA) -C $(DIR_DRIVERS)

-include $(C_DEP)
