# Current directory:
CURRENT_DIR := $(shell pwd)


# Compilation and assembly methods:
export ASM    := nasm
export CC     := x86_64-elf-gcc
export LINKER := x86_64-elf-ld

# asmbller flags: 
export ASSEMLER_MBR_FLAGS := -f bin -o
export ASSEMLER_BOOT_LOADER_FLAGS := -f elf64 -o

# compiler flags:
CURRENT_DIR                 := $(shell pwd)
INCLUDE_DIRS                := -I$(CURRENT_DIR)/src/os/ -I$(CURRENT_DIR)/src/os/src/
export COMPILER_FLAGS       := -Ttext 0x9000 -ffreestanding -mno-red-zone -m64 $(INCLUDE_DIRS)
export COMPILER_DEBUG_FLAGS := $(COMPILER_FLAGS) -DDEBUG=1

# locations:
export BIN_DIR := bin/
export SRC_DIR := src/
export OBJ_DIR := obj/
export ASM_DIR := my_asm/

# files:
export MBR_FILE        := src/mbr/mbr.asm
export MBR_BIN         := $(BIN_DIR)mbr.bin
export BOOTLOADER_FILE := src/boot_loader/bootloader.asm
export BOOTLOADER_OBJ  := $(OBJ_DIR)bootloader.o
export OS_BIN          := $(BIN_DIR)os.iso
export KERNEL_BIN      := $(BIN_DIR)kernel.bin

# this removes the ./ prefix at the start:
REMOVE_PREFIX := sed 's|^\./||'
export CPP_FILES     := $(shell find . -name "*.cpp" | $(REMOVE_PREFIX))
export OBJ_CPP_FILES := $(addprefix $(OBJ_DIR), $(CPP_FILES:.cpp=.o))
export ASM_CPP_FILES := $(addprefix $(ASM_DIR), $(CPP_FILES:.cpp=.asm))


# simulator:
export SIMULATOR := qemu-system-x86_64

# simulator flags:
RAM_SIZE   := -m 256M
CPU_TYPE   := -cpu qemu64
CLOCK_TIME := -rtc base=localtime
export SIMULATOR_FLAGS $(RAM_SIZE) $(CPU_TYPE) $(CLOCK_TIME)

#echo colors: 
export ECHO_GREEN_COLOR := \033[0;32m
export ECHO_NO_COLOR    := \033[97m