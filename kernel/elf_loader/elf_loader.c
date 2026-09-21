#include <include/integer_utils.h>
#include <include/mem_utils.h>
#include <include/types.h>
#include <include/vendor/elf.h>
#include <kernel/include/heap.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "kernel/include/elf_loader.h"

static void load_binary_segments(const Elf64_Ehdr* elf_header, struct vfs_file* elf_file,
								 void* pagemap_ptr);
static void load_segment(Elf64_Phdr* program_header, struct vfs_file* elf_file, void* pagemap_ptr);
static void print_elf64_header(const Elf64_Ehdr* ehdr);
static inline uint64_t get_elf_segment_pages_count(const Elf64_Phdr* program_header);

uint64_t load_elf_to_memory(const char* file_path, void* pagemap_ptr) {
	struct vfs_file* elf_file = vfs_open(file_path);
	Elf64_Ehdr* elf_header = kmalloc(sizeof(Elf64_Ehdr));

	elf_file->file_position = 0;
	elf_file->read(elf_file, (uint8_t*)elf_header, sizeof(Elf64_Ehdr));
	load_binary_segments(elf_header, elf_file, pagemap_ptr);

	uint64_t elf_start = elf_header->e_entry;
	kfree(elf_header);
	return elf_start;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static void load_binary_segments(const Elf64_Ehdr* elf_header, struct vfs_file* elf_file,
								 void* pagemap_ptr) {
	Elf64_Phdr* program_headers = kmalloc(sizeof(Elf64_Phdr) * elf_header->e_phnum);

	elf_file->file_position = elf_header->e_phoff;
	elf_file->read(elf_file, (uint8_t*)program_headers, sizeof(Elf64_Phdr) * elf_header->e_phnum);

	for (uint32_t i = 0; i < elf_header->e_phnum; i++) {
		Elf64_Phdr* curr_ph = &program_headers[i];
		if (curr_ph->p_type == PT_LOAD) {
			load_segment(curr_ph, elf_file, pagemap_ptr);
		}
	}
	kfree(program_headers);
}

static void load_segment(Elf64_Phdr* program_header, struct vfs_file* elf_file, void* pagemap_ptr) {
	// Mapping the segment to memory:
	uint64_t segment_page_count = get_elf_segment_pages_count(program_header);

	uint8_t* segment_vaddr = (uint8_t*)program_header->p_vaddr;
	uint8_t* phys_pages = pmm_alloc_pages(segment_page_count);
	for (uint64_t i = 0; i < segment_page_count; i++) {
		vmm_map_page(pagemap_ptr, segment_vaddr + (i * REGULAR_PAGE_SIZE),
					 phys_pages + (i * REGULAR_PAGE_SIZE), MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
	}

	// Loading the segment to memory:
	uint8_t* segment_start_hhdm_location =
		vmm_phys_to_virt_hhdm(phys_pages) + (program_header->p_vaddr % REGULAR_PAGE_SIZE);
	elf_file->file_position = program_header->p_offset;
	elf_file->read(elf_file, segment_start_hhdm_location, program_header->p_filesz);

	if (program_header->p_memsz > program_header->p_filesz) {
		memset(segment_start_hhdm_location + program_header->p_filesz, 0,
			   program_header->p_memsz - program_header->p_filesz);
	}
}

static inline uint64_t get_elf_segment_pages_count(const Elf64_Phdr* program_header) {
	uint64_t pages_block_start_vaddr = program_header->p_vaddr & (~(REGULAR_PAGE_SIZE - 1));
	uint64_t pages_block_end_vaddr =
		(program_header->p_vaddr + program_header->p_memsz + REGULAR_PAGE_SIZE - 1) &
		(~(REGULAR_PAGE_SIZE - 1));

	return (pages_block_end_vaddr - pages_block_start_vaddr) / REGULAR_PAGE_SIZE;
}
