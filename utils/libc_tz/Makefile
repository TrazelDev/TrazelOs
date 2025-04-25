SRC_FILES != find *.c
OBJ_FILES := $(addprefix $(DIR_BIN_LIB_LIBCTZ)/, $(SRC_FILES:.c=.o))

$(BIN_LIB_LIBCTZ): $(OBJ_FILES)
	$(AR) -r $@ $^

$(DIR_BIN_LIB_LIBCTZ)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $^ -o $@
