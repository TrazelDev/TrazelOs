# Current directory:
CURRENT_DIR := $(shell pwd)


# Compilation and assembly methods:
export ASM            := nasm
export CC             := x86_64-elf-gcc
export LINKER         := x86_64-elf-ld

# asmbller flags: 
export ASSEMBLER_MBR_FLAGS := -f bin -o
export ASSEMBLER_BOOT_LOADER_FLAGS := -f elf64 -o
export ASSEMBLER_OS_FLAGS := -f elf64 -o
# compiler flags:
CURRENT_DIR                   := $(shell pwd)
INCLUDE_DIRS                  := -I$(CURRENT_DIR)/src/kernel/ -I$(CURRENT_DIR)/src/kernel/src/
COMPILER_FLAGS                := -ffreestanding -mno-red-zone -m64 $(INCLUDE_DIRS)
export COMPILER_RELEASE_FLAGS := $(COMPILER_FLAGS) -O2
export COMPILER_DEBUG_FLAGS   := $(COMPILER_FLAGS) -O2 -DDEBUG=1

# locations:
export BIN_DIR     := bin/
export SRC_DIR     := src/
export OBJ_DIR     := obj/
export PROCESSES   := processes/
export ASM_DIR     := my_asm/

# files:
export MBR_FILE        := src/mbr/mbr.asm
export MBR_BIN         := $(BIN_DIR)mbr.bin
export MEMORY_PAD_FILE := src/mbr/memoryPad.asm
export MEMORY_PAD_BIN  := $(BIN_DIR)memoryPad.bin
export BOOTLOADER_FILE := src/boot_loader/bootloader.asm
export BOOTLOADER_OBJ  := $(OBJ_DIR)bootloader.o
export OS_BIN          := $(BIN_DIR)os.iso
export KERNEL_BIN      := $(BIN_DIR)kernel.bin
export PROCESS_PADDING := $(SRC_DIR)$(PROCESSES)processPadding

# this removes the ./ prefix at the start:
REMOVE_PREFIX := sed 's|^\./||'
export CPP_FILES         := $(shell find $(SRC_DIR)kernel -name "*.cpp" | $(REMOVE_PREFIX)) 
export ASM_FILES         := $(shell find $(SRC_DIR)kernel -name "*.asm" | $(REMOVE_PREFIX))
export OBJ_CPP_FILES     := $(addprefix $(OBJ_DIR), $(CPP_FILES:.cpp=.o)   )
export OBJ_ASM_FILES     := $(addprefix $(OBJ_DIR), $(ASM_FILES:.asm=Asm.o))
export ASM_CPP_FILES     := $(addprefix $(ASM_DIR), $(CPP_FILES:.cpp=.asm) )

# process files
export PROCESSES_LIST    ?= $(addsuffix .bin, $(shell find $(PROCESSES)obj -maxdepth 1 -mindepth 1 -type d | sed 's|^$(PROCESSES)obj/|bin/processes/|'))
export PROCESS_CPP_FILES := 
export OBJ_PROCESSES     := 


# simulator:
export SIMULATOR := qemu-system-x86_64

# simulator flags:
RAM_SIZE   := -m 256M
CPU_TYPE   := -cpu qemu64
CLOCK_TIME := -rtc base=localtime
DEVICE     := -device piix3-ide,id=ide -device ide-hd,drive=disk,bus=ide.0
export SIMULATOR_FLAGS $(RAM_SIZE) $(CPU_TYPE) $(CLOCK_TIME) $(DEVICE)

#echo colors: 
export ECHO_GREEN_COLOR := \033[0;32m
export ECHO_NO_COLOR    := \033[97m


SECTOR_SIZE             := 512
PAGE_SIZE               := 0x1000
KERNEL_SIZE             ?= $(shell wc -c < $(KERNEL_BIN))
PROCESSES_COMBINED_SIZE ?= $$(du -cb $(PROCESSES_LIST) | awk 'END{print $$1}')
ALL_BINARIES_SIZE       ?= $$(($(KERNEL_SIZE) + $(PROCESSES_COMBINED_SIZE)))
export SECTORS_TO_LOAD  ?= $(shell if [ -e "$(KERNEL_BIN)" ]; then echo $$((($(ALL_BINARIES_SIZE) / $(SECTOR_SIZE)))); fi)

# this is the memory that is need to be padded so it will be exactly the right amount of memory that should be loaded:
export MEMORY_PAD ?= $(shell if [ -e "$(KERNEL_BIN)" ]; then echo $$(($(SECTOR_SIZE) - ($(KERNEL_SIZE) % $(SECTOR_SIZE)))); fi)