$(BIN_USER_INIT): init.c $(DIR_BIN_USER)
	$(MAKE) -C doomgeneric/doomgeneric
	$(CC) -c init.c -o $(DIR_BIN_USER)/init.o
	$(CC) -o $(BIN_USER_INIT) $(DIR_BIN_USER)/init.o doomgeneric/doomgeneric/build/*.o

# $(BIN_USER_INIT): init.c $(DIR_BIN_USER)
# 	$(CC) $(CC_PATH_MAP_FLAGS) -ggdb3 init.c -o $(BIN_USER_INIT)

$(DIR_BIN_USER):
	mkdir -p $(DIR_BIN_USER)
