include commons.mk

SUBDIRS := boot/mbr boot/stage2
COMMONS_PATH := $(abspath commons.mk)

all: build
build: build_submodules
	cat bin/mbr.bin bin/stage2_boot.bin > bin/os.iso


build_submodules:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir COMMONS_PATH=$(COMMONS_PATH); \
	done
