# TODO create the actual ata pio mode driver makefile than link the binnary 
# into to the bootloader and test it


SOURCE_FILES != find *.c
OBJ_FILES := $(addprefix $(DIR_BIN_DRIVER_ATA_PIO)/, $(SOURCE_FILES:.c=.o))
DEP_FILES := $(addprefix $(DIR_BIN_DRIVER_ATA_PIO)/, $(SOURCE_FILES:.c=.d))

$(BIN_DRIVER_ATA_PIO): $(OBJ_FILES) $(BIN_IO)
	$(AR) r $@ $^

$(DIR_BIN_DRIVER_ATA_PIO)/%.o: %.c $(DIR_BIN_DRIVER_ATA_PIO)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(DIR_BIN_DRIVER_ATA_PIO):
	mkdir -p $@

-include $(DEP_FILES)
