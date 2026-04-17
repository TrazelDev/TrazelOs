.PHONY: $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_DRIVER_PS2_KEYBOARD) $(BIN_LIB_LIBCTZ)
SRC_FILES := $(shell find . -name "*.c")
OBJ_FILES := $(addprefix $(DIR_BIN_KERNEL)/, $(SRC_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_KERNEL)/, $(SRC_FILES:.c=.d))
ASM_FILES := $(shell find . -name "*.asm")
ASM_OBJ_FILES := $(addprefix $(DIR_BIN_KERNEL)/, $(ASM_FILES:.asm=_asm.o))
KERNEL_LINKER := kernel_linker.ld

$(BIN_KERNEL): $(OBJ_FILES) $(ASM_OBJ_FILES) $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_DRIVER_SERIAL_COM) $(BIN_DRIVER_PS2_KEYBOARD) $(BIN_LIB_LIBCTZ)
	$(LD) -T $(KERNEL_LINKER) -z max-page-size=0x1000 -o $@ $^

$(OBJ_FILES): $(DIR_BIN_KERNEL)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -ggdb3 -c $< -o $@
	$(CC) $(CC_FLAGS) -S $< -o $(DIR_BIN_KERNEL)/$*.s

$(ASM_OBJ_FILES): $(DIR_BIN_KERNEL)/%_asm.o: %.asm
	@mkdir -p $(dir $@)
	nasm -f elf64 -g -F dwarf $< -o $@

$(BIN_DRIVER_PRINT_VGA):
	$(MAKE) $(BIN_DRIVER_PRINT_VGA) -C $(DIR_DRIVERS)

$(BIN_DRIVER_ATA_PIO):
	$(MAKE) $(BIN_DRIVER_ATA_PIO) -C $(DIR_DRIVERS)

$(BIN_DRIVER_SERIAL_COM):
	$(MAKE) $(BIN_DRIVER_SERIAL_COM) -C $(DIR_DRIVERS)

$(BIN_DRIVER_PS2_KEYBOARD):
	$(MAKE) $(BIN_DRIVER_PS2_KEYBOARD) -C $(DIR_DRIVERS)

$(BIN_LIB_LIBCTZ):
	$(MAKE) $(BIN_LIB_LIBCTZ) -C $(DIR_UTILS)

-include $(DEP_FILES)
