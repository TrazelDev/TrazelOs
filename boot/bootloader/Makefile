# making sure that binnary path is specified
ifndef COMMONS_PATH
$(error COMMONS_PATH must be specified)
endif
include $(COMMONS_PATH)

all: build

build:
	$(ASM) bootloader.asm -f bin -o $(BIN_DIR)/bootloader.bin
