include config.mk      # defintions of flags and methods that the project is using


# depends on the run so when we use just make we call this:
all: run
	

# running the os:
run: create_os_bin
	$(SIMULATOR) -drive file=$(OS_BIN),format=raw $(SIMULATOR_FLAGS)


# cleaning all of the object files and bin files:
clean:
	$(RM) -r bin/* obj/*

# adding togther the kernel and the master boot record:
create_os_bin: link
	cat $(MBR_BIN) $(KERNEL_BIN) > $(OS_BIN)


# linking all of the cpp object files and the bootloader:
link: compile $(OBJ_CPP_FILES)
	@echo "${ECHO_GREEN_COLOR}linking everything:${ECHO_NO_COLOR}"
	$(LINKER) -T linker.ld -o $(KERNEL_BIN) $(BOOTLOADER_OBJ) $(OBJ_CPP_FILES)
	@echo "\n"

# compiling the master boot record and bootloader:
compile:
	@echo "${ECHO_GREEN_COLOR}compiling mbr:${ECHO_NO_COLOR}"
	$(ASM) $(MBR_FILE) $(ASSEMLER_MBR_FLAGS) $(MBR_BIN)
	$(ASM) $(BOOTLOADER_FILE) $(ASSEMLER_BOOT_LOADER_FLAGS) $(BOOTLOADER_OBJ)
	@echo "\n"

# this tells make how to compile a cpp file into an o file:
$(OBJ_DIR)%.o: %.cpp
	@echo "${ECHO_GREEN_COLOR}compiling $<:${ECHO_NO_COLOR}"
	mkdir -p $(dir $@) # creating the dir in the case that it is not created:
	$(CC) $(COMPILER_FLAGS) -c $< -o $@
	@echo "\n"

# simple dependecies:
dependencies:
	sudo apt update
	sudo apt install qemu-system-x86
	sudo apt install nasm
	
#include dependencies.mk # installing the compiler and linker to the machine