$(BIN_USER_INIT): init.c $(DIR_BIN_USER)
	$(CC) $(CC_PATH_MAP_FLAGS) -ggdb3 -c init.c -o $(DIR_BIN_USER)/init.o
	$(LD) $(DIR_BIN_USER)/init.o -o $(BIN_USER_INIT)

$(DIR_BIN_USER):
	mkdir -p $(DIR_BIN_USER)
