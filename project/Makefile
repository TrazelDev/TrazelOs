include config.mk      # defintions of flags and methods that the project is using


# depends on the run so when we use just make we call this:
all: #run
	@echo "$(ASM_CPP_FILES)"

# a configuration to generate asm files from the cpp files:
compile_asm: $(ASM_CPP_FILES)

$(ASM_DIR)%.asm: %.cpp
	@echo "${ECHO_GREEN_COLOR}compiling $< to asm:${ECHO_NO_COLOR}"
	mkdir -p $(dir $@) # creating the dir in the case that it is not created:
	$(CC) $(COMPILER_DEBUG_FLAGS) -S $< -o $@
	@echo "\n"






# new configuration that changes the compiler flags to debug flags and the running normal run
debug: COMPILER_RELEASE_FLAGS = $(COMPILER_DEBUG_FLAGS)
debug: run





# cleaning all of the object files and bin files:
clean:
	$(RM) -r $(BIN_DIR)* $(OBJ_DIR)* $(ASM_DIR)*





# running the os:
run: create_os_bin
	$(SIMULATOR) -drive file=$(OS_BIN),format=raw $(SIMULATOR_FLAGS)

# adding togther the kernel and the master boot record:
create_os_bin: link
	cat $(MBR_BIN) $(KERNEL_BIN) > $(OS_BIN)


# linking all of the cpp object files and the bootloader:
link: asm compile assemble
	@echo "${ECHO_GREEN_COLOR}linking everything:${ECHO_NO_COLOR}"
	$(LINKER) -T linker.ld -o $(KERNEL_BIN) $(BOOTLOADER_OBJ) $(OBJ_ASM_FILES) $(OBJ_CPP_FILES)
	@echo "\n"

compile: $(OBJ_CPP_FILES) 

assemble: $(OBJ_ASM_FILES)

# assembling the master boot record and bootloader:
asm:
	@echo "${ECHO_GREEN_COLOR}assembling mbr:${ECHO_NO_COLOR}"
	$(ASM) $(MBR_FILE) $(ASSEMBLER_MBR_FLAGS) $(MBR_BIN)
	$(ASM) $(BOOTLOADER_FILE) $(ASSEMBLER_BOOT_LOADER_FLAGS) $(BOOTLOADER_OBJ)
	@echo "\n"

# this tells make how to compile a cpp file into an o file:
$(OBJ_DIR)%.o: %.cpp
	@echo "${ECHO_GREEN_COLOR}compiling $<:${ECHO_NO_COLOR}"
	mkdir -p $(dir $@) # creating the dir in the case that it is not created:
	$(CC) $(COMPILER_RELEASE_FLAGS) -c $< -o $@
	@echo "\n"

# this tells make how to assemble a asm file into an o file:
$(OBJ_DIR)%Asm.o: %.asm
	@echo "${ECHO_GREEN_COLOR}assembling $<:${ECHO_NO_COLOR}"
	mkdir -p $(dir $@) # creating the dir in the case that it is not created:
	$(ASM) $< $(ASSEMBLER_BOOT_LOADER_FLAGS) $@
	@echo "\n"



# simple dependecies:
dependencies:
	sudo apt update
	sudo apt install qemu-system-x86
	sudo apt install nasm