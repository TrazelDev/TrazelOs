# Tools:
export ASM = nasm
export CC = x86_64-elf-gcc
export LD = x86_64-elf-ld


# Path of this directory:
CURR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
CURR_DIR := $(strip $(CURR_DIR))

# binnaries path
export BIN_DIR := $(CURR_DIR)bin
export MBR_BIN := $(BIN_DIR)/mbr.bin
export BOOTLOADER_BIN := $(BIN_DIR)/bootloader.bin
