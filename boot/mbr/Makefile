# making sure that binnary path is specified
ifndef COMMONS_PATH
$(error COMMONS_PATH must be specified)
endif
include $(COMMONS_PATH)

all: build

build:
	$(ASM) mbr.asm -f bin -o $(MBR_BIN)
