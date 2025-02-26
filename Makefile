SUBDIRS := boot/mbr

all: build
build: build_submodules


build_submodules: 
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir BIN_PATH=$(BIN_PATH); \
	done
