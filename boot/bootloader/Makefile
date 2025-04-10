# Making sure that binnary path is specified
ifndef COMMONS_PATH
$(error COMMONS_PATH must be specified)
endif
include $(COMMONS_PATH)

BOOT_BIN_DIR := $(BIN_DIR)/boot

# -ffreestanding - says that there are no stanard libs
# -mno-red-zone - something with stack just in gcc, man it
# -m64 - size of values like int long and so on
# -c - do not link
COMPILER_FLAGS := -ffreestanding -mno-red-zone -m64 -c

all: build
build:
	mkdir -p $(BOOT_BIN_DIR)
	$(ASM) bootloader.asm -f elf64 -o $(BOOT_BIN_DIR)/asm_bootloader.o
	$(CC) $(COMPILER_FLAGS) bootloader.c -o $(BOOT_BIN_DIR)/c_bootloader.o
	$(LD) -T linker.ld -o $(BOOTLOADER_BIN) $(BOOT_BIN_DIR)/asm_bootloader.o $(BOOT_BIN_DIR)/c_bootloader.o
