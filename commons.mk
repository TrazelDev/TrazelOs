# Path of this directory:
CURR_DIR != pwd

# Tools:
export ASM = nasm
export CC = x86_64-elf-gcc
export LD = x86_64-elf-ld

# compiler global setting:
export GLOBAL_INCLUDE_DIR := $(CURR_DIR)/include
export DRIVERS_DIR := $(CURR_DIR)/drivers

# Common compiler flags:
# ------------------------------------------------------------------
# -ffreestanding - says that there are no stanard libs
# -mno-red-zone - something with stack just in gcc, man it
# -m64 - size of values like int long and so on
export CC_FLAGS := -ffreestanding -mno-red-zone -m64 -I $(GLOBAL_INCLUDE_DIR)
# ------------------------------------------------------------------


# binnaries path
export BIN_DIR := $(CURR_DIR)/bin
export MBR_BIN := $(BIN_DIR)/mbr.bin
export BOOTLOADER_BIN := $(BIN_DIR)/bootloader.bin
export DRIVERS_BIN_DIR := $(BIN_DIR)/drivers
export UTILS_BIN_DIR := $(BIN_DIR)/utils
export OS_IMG := $(BIN_DIR)/os.img
