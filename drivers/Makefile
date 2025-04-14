# Making sure that project common settings are specified:
ifndef COMMONS_PATH
$(error COMMONS_PATH must be specified)
endif
include $(COMMONS_PATH)



SUBDIRS := $(ls -d */)

all: build
build:
	mkdir -p $(DRIVERS_BIN_DIR)
	$(CC) $(CC_FLAGS) -c io.c -o $(DRIVERS_BIN_DIR)/io.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir COMMONS_PATH=$(COMMONS_PATH); \
	done
