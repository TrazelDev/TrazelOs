include commons.mk

SUBDIRS := boot/mbr boot/bootloader
COMMONS_PATH := $(abspath commons.mk)

# The main options:
all: build
run:
	sudo qemu-system-x86_64 bin/os.iso
clean:
	rm -rf bin


build: build_submodules
	cat $(MBR_BIN) $(BOOTLOADER_BIN) > bin/os.iso


build_submodules:
	mkdir -p bin
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir COMMONS_PATH=$(COMMONS_PATH); \
	done
