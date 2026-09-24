.PHONY: force
force: ;

$(DIR_BIN_DRIVERS)/%.a: $(BIN_IO) $(BIN_BLOCK_DEVICE) $(BIN_CHAR_DEVICE) force
	$(MAKE) $@ -C $*

$(BIN_IO): $(DIR_DRIVERS)/io.c
	mkdir -p $(DIR_BIN_DRIVERS)
	$(CC) $(CC_FLAGS) -c $(DIR_DRIVERS)/io.c -o $(BIN_IO)

$(BIN_BLOCK_DEVICE): $(DIR_DRIVERS)/block_device.c
	mkdir -p $(DIR_BIN_DRIVERS)
	$(CC) $(CC_FLAGS) -c $(DIR_DRIVERS)/block_device.c -o $(BIN_BLOCK_DEVICE)

$(BIN_CHAR_DEVICE): $(DIR_DRIVERS)/char_device.c
	mkdir -p $(DIR_BIN_DRIVERS)
	$(CC) $(CC_FLAGS) -c $(DIR_DRIVERS)/char_device.c -o $(BIN_CHAR_DEVICE)
