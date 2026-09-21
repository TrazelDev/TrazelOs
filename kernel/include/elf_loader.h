#pragma once
#include <include/types.h>

/** Loads the ELF file at the specified path into memory and maps it to the provided pagemap
 * pointer.
 * @param file_path The path to the ELF file to load.
 * @param pagemap_ptr A pointer to the pagemap where the ELF file will be loaded.
 * @return The entry point address of the loaded ELF file.
 */
uint64_t load_elf_to_memory(const char* file_path, void* pagemap_ptr);
