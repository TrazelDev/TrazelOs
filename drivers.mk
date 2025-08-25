include commons.mk

DRIVER_NAMES := $(shell find $(DIR_DRIVERS) -mindepth 1 -maxdepth 1 -type d -printf "%f ")
BIN_ALL_DRIVERS := $(addprefix $(DIR_BIN_DRIVERS)/, $(DRIVER_NAMES))
export BIN_ALL_DRIVERS := $(addsuffix .a, $(BIN_ALL_DRIVERS))


PRINT_VGA := vga_text
export DIR_DRIVER_PRINT_VGA := $(DIR_DRIVERS)/$(PRINT_VGA)
export DIR_BIN_DRIVER_PRINT_VGA := $(DIR_BIN_DRIVERS)/$(PRINT_VGA)
export BIN_DRIVER_PRINT_VGA := $(DIR_BIN_DRIVERS)/$(PRINT_VGA).a


ATA_PIO := ata_pio
export DIR_DRIVER_ATA_PIO := $(DIR_DRIVERS)/$(ATA_PIO)
export DIR_BIN_DRIVER_ATA_PIO := $(DIR_BIN_DRIVERS)/$(ATA_PIO)
export BIN_DRIVER_ATA_PIO := $(DIR_BIN_DRIVERS)/$(ATA_PIO).a

