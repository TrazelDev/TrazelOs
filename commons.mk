# tools:
export ASM = nasm



# path of this directory:
CURR_DIR := $(dir $(lastword $(MAKEFILE_LIST))) 
CURR_DIR := $(strip $(CURR_DIR))

export BIN_DIR := $(CURR_DIR)bin/

