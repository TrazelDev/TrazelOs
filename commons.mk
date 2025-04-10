# Path of this directory:
CURR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
CURR_DIR := $(strip $(CURR_DIR))

# Tools:
export ASM = nasm
export CC = x86_64-elf-gcc
export LD = x86_64-elf-ld

# compiler global setting:
export GLOBAL_INCLUDE_DIR = $(CURR_DIR)utility


# binnaries path
export BIN_DIR := $(CURR_DIR)bin
export MBR_BIN := $(BIN_DIR)/mbr.bin
export BOOTLOADER_BIN := $(BIN_DIR)/bootloader.bin
export OS_IMG := $(BIN_DIR)/os.img
