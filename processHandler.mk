include config.mk      # defintions of flags and methods that the project is using

.PHONY: link_processes compile_processes build_processes create_process_lists
link_processes: compile_processes
	$(eval PROCESSES_LIST    := $(addsuffix .bin, $(shell find $(PROCESSES)obj -maxdepth 1 -mindepth 1 -type d | sed 's|^$(PROCESSES)obj/|bin/processes/|')))
	@$(MAKE) $(PROCESSES_LIST)

compile_processes: create_process_lists
	@$(MAKE) $(OBJ_PROCESSES)

create_process_lists: build_processes
	$(eval PROCESS_CPP_FILES := $(shell find $(PROCESSES) -name "*.cpp" | $(REMOVE_PREFIX)))
	$(eval OBJ_PROCESSES     := $(subst .cpp,.o,$(subst code/,obj/,$(PROCESS_CPP_FILES))))

build_processes:
	python3 scripts/copyAllProcesses.py

# creating process cpp file:
$(PROCESSES)obj/%.o: $(PROCESSES)code/%.cpp
	@echo "${ECHO_GREEN_COLOR}compiling $<:${ECHO_NO_COLOR}"
	mkdir -p $(dir $@)
	$(CC) $(COMPILER_RELEASE_FLAGS) -c $< -o $@
	@echo "\n"


bin/processes/%.bin:
	$(eval PROCESSES_LIST    := $(addsuffix .bin, $(shell find $(PROCESSES)obj -maxdepth 1 -mindepth 1 -type d | sed 's|^$(PROCESSES)obj/|bin/processes/|')))
	$(eval PROCESS_CPP_FILES := $(shell find $(PROCESSES) -name "*.cpp" | $(REMOVE_PREFIX)))
	$(eval OBJ_PROCESSES     := $(subst .cpp,.o,$(subst code/,obj/,$(PROCESS_CPP_FILES))))
	$(eval PROCESS_OBJ_FILES := $(foreach obj,$(OBJ_PROCESSES),$(if $(findstring $*,$(obj)),$(obj))))
	
	@echo "${ECHO_GREEN_COLOR}linking $@:${ECHO_NO_COLOR}"
	mkdir -p $(dir $@)
	$(LINKER) -T $(PROCESSES)linker.ld -o $@ $(PROCESS_OBJ_FILES)

	
