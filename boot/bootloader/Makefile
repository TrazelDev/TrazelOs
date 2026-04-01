.PHONY: $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_LIB_LIBCTZ)

ASM_BIN := $(DIR_BIN_BOOT)/asm_bootloader.o
SRC_FILES := $(shell find . -name "*.c")
OBJ_FILES := $(addprefix $(DIR_BIN_BOOT)/, $(SRC_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_BOOT)/, $(SRC_FILES:.c=.d))

$(BIN_BOOTLOADER): $(ASM_BIN) $(OBJ_FILES) $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_LIB_LIBCTZ)
	$(LD) -T linker.ld -o $(BIN_BOOTLOADER) $^

$(ASM_BIN): bootloader.asm $(DIR_BIN_BOOT)
	$(ASM) $< -f elf64 -o $@

$(DIR_BIN_BOOT)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_BOOT):
	mkdir -p $(DIR_BIN_BOOT)

$(BIN_DRIVER_PRINT_VGA):
	$(MAKE) $(BIN_DRIVER_PRINT_VGA) -C $(DIR_DRIVERS)

$(BIN_DRIVER_ATA_PIO):
	$(MAKE) $(BIN_DRIVER_ATA_PIO) -C $(DIR_DRIVERS)

$(BIN_LIB_LIBCTZ):
	$(MAKE) $(BIN_LIB_LIBCTZ) -C $(DIR_UTILS)
	
-include $(DEP_FILES)
