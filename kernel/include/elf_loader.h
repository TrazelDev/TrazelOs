#pragma once
#include <include/types.h>
#include <kernel/include/vfs.h>

/** Loads the ELF file at the specified path into memory and maps it to the provided pagemap
 * pointer.
 * @param file_path The path to the ELF file to load.
 * @param pagemap_ptr A pointer to the pagemap where the ELF file will be loaded.
 * @return The entry point address of the loaded ELF file or NULL in the case of a fail
 */
uint64_t load_elf_to_memory(struct vfs_file* elf_file, void* pagemap_hhdm_ptr);
