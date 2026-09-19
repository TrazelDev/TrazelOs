SOURCE_FILES != find *.c
OBJ_FILES := $(addprefix $(DIR_BIN_DRIVER_VFS_FAT12)/, $(SOURCE_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_DRIVER_VFS_FAT12)/, $(SOURCE_FILES:.c=.d))

$(BIN_DRIVER_VFS_FAT12): $(OBJ_FILES) $(BIN_IO) $(BIN_BLOCK_DEVICE)
	$(AR) r $@ $^

$(DIR_BIN_DRIVER_VFS_FAT12)/%.o: %.c $(DIR_BIN_DRIVER_VFS_FAT12)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_DRIVER_VFS_FAT12):
	mkdir -p $@

-include $(DEP_FILES)
