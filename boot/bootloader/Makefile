# Making sure that project common settings are specified:
ifndef COMMONS_PATH
$(error COMMONS_PATH must be specified)
endif
include $(COMMONS_PATH)

BOOT_BIN_DIR := $(BIN_DIR)/boot

all: build
build:
	mkdir -p $(BOOT_BIN_DIR)
	$(ASM) bootloader.asm -f elf64 -o $(BOOT_BIN_DIR)/asm_bootloader.o
	$(CC) $(CC_FLAGS) -c bootloader.c -o $(BOOT_BIN_DIR)/c_bootloader.o
	$(LD) -T linker.ld -o $(BOOTLOADER_BIN) $(BOOT_BIN_DIR)/asm_bootloader.o $(BOOT_BIN_DIR)/c_bootloader.o
