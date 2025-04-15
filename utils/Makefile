UTILS_SOURCE_FILES := $(shell find *.c)
# List of all the binnaries that I want to generate 
# UTILS_OBJ_FILES := $(addprefix $() $(UTILS_SOURCE_FILES:.c=.o)
UTILS_OBJ_FILES := $(UTILS_SOURCE_FILES:.c=.o)

all: build
build: $(UTILS_OBJ_FILES)
	mkdir -p $(UTILS_BIN_DIR)
	echo $(C)

%.o: %.c
	echo $<
