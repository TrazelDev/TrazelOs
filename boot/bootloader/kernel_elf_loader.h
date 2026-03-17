#pragma once
#include <include/elf.h>

int load_and_jump_kernel(void* kernel_elf_data, size_t kernel_elf_size);
