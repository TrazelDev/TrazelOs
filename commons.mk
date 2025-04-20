DIR_CURR != pwd

# important directories:
export DIR_MBR := $(DIR_CURR)/boot/mbr
export DIR_BOOTLOADER := $(DIR_CURR)/boot/bootloader
export DIR_DRIVERS := $(DIR_CURR)/drivers
export DIR_INCLUDS := $(DIR_CURR)/include
export DIR_UTILS := $(DIR_CURR)/utils

# bin dirs:
export DIR_BIN := $(DIR_CURR)/bin
export DIR_BIN_BOOT := $(DIR_BIN)/boot
export DIR_BIN_DRIVERS := $(DIR_BIN)/drivers
export DIR_BIN_UTILS := $(DIR_BIN)/utils

# bin files:
export OS_IMG := $(DIR_BIN)/os.img
export BIN_MBR := $(DIR_BIN)/mbr.bin
export BIN_BOOTLOADER := $(DIR_BIN)/bootloader.bin
export BIN_IO := $(DIR_BIN_DRIVERS)/io.o



# tools:
export ASM = nasm
export CC = x86_64-elf-gcc
export LD = x86_64-elf-ld

# common compiler flags:
# ------------------------------------------------------------------
# -ffreestanding - says that there are no stanard libs
# -mno-red-zone - something with stack just in gcc, man it
# -m64 - size of values like int long and so on
export CC_FLAGS := -ffreestanding -mno-red-zone -m64 -I $(DIR_INCLUDS)
# ------------------------------------------------------------------
