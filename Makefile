include commons.mk

SUBDIRS := boot/mbr boot/bootloader drivers utils
COMMONS_PATH := $(abspath commons.mk)

# The main options:
all: build
run:
	sudo qemu-system-x86_64 $(OS_IMG)
clean:
	rm -rf bin


build: build_submodules
	dd if=/dev/zero of=$(OS_IMG) count=100
	dd if=$(MBR_BIN) of=$(OS_IMG) count=1 conv=notrunc
	dd if=$(BOOTLOADER_BIN) of=$(OS_IMG) seek=1 conv=notrunc
	echo ', 5, L, *' | sfdisk $(OS_IMG)


build_submodules:
	mkdir -p bin
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir COMMONS_PATH=$(COMMONS_PATH); \
	done
