# Making sure that binnary path is specified
ifndef COMMONS_PATH
$(error COMMONS_PATH must be specified)
endif
include $(COMMONS_PATH)

# -ffreestanding - says that there are no stanard libs
# -mno-red-zone - something with stack just in gcc, man it
# -m64 - size of values like int long and so on
# -c - do not link
COMPILER_FLAGS := -ffreestanding -mno-red-zone -m64 -c -I $(GLOBAL_INCLUDE_DIR)
DRIVERS_BIN_DIR := $(BIN_DIR)/drivers


SUBDIRS := $(ls -d */)
all: build

build:
	mkdir -p $(DRIVERS_BIN_DIR)
	$(CC) $(COMPILER_FLAGS) io.c -o $(DRIVERS_BIN_DIR)/io.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir COMMONS_PATH=$(COMMONS_PATH); \
	done
