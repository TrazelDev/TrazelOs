SOURCE_FILES != find *.c
OBJ_FILES := $(addprefix $(DIR_BIN_DRIVER_PRINT_VGA)/, $(SOURCE_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_DRIVER_PRINT_VGA)/, $(SOURCE_FILES:.c=.d))

$(BIN_DRIVER_PRINT_VGA): $(OBJ_FILES) $(BIN_IO)
	$(AR) r $@ $^

$(DIR_BIN_DRIVER_PRINT_VGA)/%.o: %.c $(DIR_BIN_DRIVER_PRINT_VGA)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_DRIVER_PRINT_VGA):
	mkdir -p $@

-include $(DEP_FILES)
