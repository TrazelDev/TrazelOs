$(BIN_USER_INIT): init.asm $(DIR_BIN_USER)
	nasm -f elf64 -g -F dwarf init.asm -o $(DIR_BIN_USER)/init.o
	$(LD) $(DIR_BIN_USER)/init.o -o $(BIN_USER_INIT)

$(DIR_BIN_USER):
	mkdir -p $(DIR_BIN_USER)
