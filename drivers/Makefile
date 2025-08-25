.PHONY: force
force: ;

$(DIR_BIN_DRIVERS)/%.a: $(BIN_IO) force
	$(MAKE) $@ -C $*

$(BIN_IO): $(DIR_DRIVERS)/io.c
	mkdir -p $(DIR_BIN_DRIVERS)
	$(CC) $(CC_FLAGS) -c $(DIR_DRIVERS)/io.c -o $(BIN_IO)
