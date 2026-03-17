SRC_FILES != find *.c
OBJ_FILES := $(addprefix $(DIR_BIN_KERNEL)/, $(SRC_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_KERNEL)/, $(SOURCE_FILES:.c=.d))
KERNEL_LINKER := kernel_linker.ld

$(BIN_KERNEL): $(OBJ_FILES)
	$(LD) -T $(KERNEL_LINKER) -z max-page-size=0x1000 -o $@ $^

$(DIR_BIN_KERNEL)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -ggdb3 -c $^ -o $@


-include $(DEP_FILES)
