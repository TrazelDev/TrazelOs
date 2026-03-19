SOURCE_FILES != find *.c
OBJ_FILES := $(addprefix $(DIR_BIN_DRIVER_SERIAL_COM)/, $(SOURCE_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_DRIVER_SERIAL_COM)/, $(SOURCE_FILES:.c=.d))

$(BIN_DRIVER_SERIAL_COM): $(OBJ_FILES) $(BIN_IO)
	$(AR) r $@ $^

$(DIR_BIN_DRIVER_SERIAL_COM)/%.o: %.c $(DIR_BIN_DRIVER_SERIAL_COM)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_DRIVER_SERIAL_COM):
	mkdir -p $@

-include $(DEP_FILES)
