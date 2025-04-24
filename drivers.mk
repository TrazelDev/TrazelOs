include commons.mk

export DRIVER_NAMES != ls $(DIR_DRIVERS) -d
export BIN_ALL_DRIVERS := $(addprefix $(DIR_BIN_DRIVERS), $(addsuffix .a, $(DRIVER_NAMES)))


PRINT_VGA := vga_text
export DIR_DRIVER_PRINT_VGA := $(DIR_DRIVERS)/$(PRINT_VGA)
export DIR_BIN_DRIVER_PRINT_VGA := $(DIR_BIN_DRIVERS)/$(PRINT_VGA)
export BIN_DRIVER_PRINT_VGA := $(DIR_BIN_DRIVERS)/$(PRINT_VGA).a
