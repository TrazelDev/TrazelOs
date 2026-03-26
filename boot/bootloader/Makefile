.PHONY: $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_LIB_LIBCTZ)

ASM_BIN := $(DIR_BIN_BOOT)/asm_bootloader.o
C_BIN := $(DIR_BIN_BOOT)/c_bootloader.o
C_DEP := $(DIR_BIN_BOOT)/c_bootloader.d
PARTITION_TABLE_BIN := $(DIR_BIN_BOOT)/partition_table.o
PARTITION_TABLE_DEP := $(DIR_BIN_BOOT)/partition_table.d
ALLOCATOR_BIN := $(DIR_BIN_BOOT)/bootloader_alloc.o
ALLOCATOR_DEP := $(DIR_BIN_BOOT)/bootloader_alloc.d
FAT12_BIN := $(DIR_BIN_BOOT)/fat12.o
FAT12_DEP := $(DIR_BIN_BOOT)/fat12.d
ELF_LOADER_BIN := $(DIR_BIN_BOOT)/kernel_elf_loader.o
ELF_LOADER_DEP := $(DIR_BIN_BOOT)/kernel_elf_loader.d
BOOT_PRINT_BIN := $(DIR_BIN_BOOT)/bootloader_print.o
BOOT_PRINT_DEP := $(DIR_BIN_BOOT)/bootloader_print.d
HHDM_BIN := $(DIR_BIN_BOOT)/hhdm.o
HHDM_DEP := $(DIR_BIN_BOOT)/hhdm.d

$(BIN_BOOTLOADER): $(ASM_BIN) $(C_BIN) $(PARTITION_TABLE_BIN) $(ALLOCATOR_BIN) $(FAT12_BIN) $(ELF_LOADER_BIN) $(BOOT_PRINT_BIN) $(HHDM_BIN) $(BIN_DRIVER_PRINT_VGA) $(BIN_DRIVER_ATA_PIO) $(BIN_LIB_LIBCTZ) 
	$(LD) -T linker.ld -o $(BIN_BOOTLOADER) $^

$(ASM_BIN): bootloader.asm $(DIR_BIN_BOOT)
	$(ASM) $< -f elf64 -o $@

$(C_BIN): bootloader.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(PARTITION_TABLE_BIN): partition_table.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(ALLOCATOR_BIN): bootloader_alloc.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(FAT12_BIN): fat12.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(ELF_LOADER_BIN): kernel_elf_loader.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(BOOT_PRINT_BIN): bootloader_print.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(HHDM_BIN): hhdm.c $(DIR_BIN_BOOT)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_BOOT):
	mkdir -p $(DIR_BIN_BOOT)

$(BIN_DRIVER_PRINT_VGA):
	$(MAKE) $(BIN_DRIVER_PRINT_VGA) -C $(DIR_DRIVERS)

$(BIN_DRIVER_ATA_PIO):
	$(MAKE) $(BIN_DRIVER_ATA_PIO) -C $(DIR_DRIVERS)

$(BIN_LIB_LIBCTZ):
	$(MAKE) $(BIN_LIB_LIBCTZ) -C $(DIR_UTILS)
	
-include $(C_DEP) $(PARTITION_TABLE_DEP) $(ALLOCATOR_DEP) $(FAT12_DEP) $(ELF_LOADER_DEP) $(BOOT_PRINT_DEP) $(HHDM_DEP)
