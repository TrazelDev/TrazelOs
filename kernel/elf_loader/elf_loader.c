#include <include/integer_utils.h>
#include <include/mem_utils.h>
#include <include/types.h>
#include <include/vendor/elf.h>
#include <kernel/include/heap.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "kernel/include/elf_loader.h"

#define ALIGN_UP(x, align) (((x) % (align) == 0) ? (x) : ((x) + (align) - ((x) % (align))))

/** Makes sure the magic bytes verify the file is an elf file */
static bool is_elf_file(Elf64_Ehdr* elf_header);
static Elf64_Phdr* get_program_headers(Elf64_Ehdr* elf_header, struct vfs_file* elf_file);
/** Iterates through the program headers and finds the highest place something is loaded and returns
 * the address of the page following it which will be the place for the heap */
static uint64_t find_heap_start(Elf64_Phdr* program_headers, size_t ph_count);
static void load_binary_segments(Elf64_Phdr* program_headers, size_t ph_count,
								 struct vfs_file* elf_file, void* pagemap_ptr);
static void load_segment(Elf64_Phdr* program_header, struct vfs_file* elf_file, void* pagemap_ptr);
static inline uint64_t get_elf_segment_pages_count(const Elf64_Phdr* program_header);

struct elf_file_info* load_elf_to_memory(struct vfs_file* elf_file, void* pagemap_hhdm_ptr) {
	Elf64_Ehdr* elf_header = kmalloc(sizeof(Elf64_Ehdr));

	vfs_read(elf_file, (uint8_t*)elf_header, sizeof(Elf64_Ehdr));
	if (!is_elf_file(elf_header)) {
		kfree(elf_header);
		return NULL;
	}

	Elf64_Phdr* program_headers = get_program_headers(elf_header, elf_file);
	load_binary_segments(program_headers, elf_header->e_phnum, elf_file, pagemap_hhdm_ptr);

	struct elf_file_info* file_info = kmalloc(sizeof(struct elf_file_info));
	file_info->entry_point = elf_header->e_entry;
	file_info->heap_start = find_heap_start(program_headers, elf_header->e_phnum);

	kfree(elf_header);
	kfree(program_headers);
	if (file_info->entry_point == NULL || file_info->heap_start == NULL) {
		kfree(file_info);
		return NULL;
	}
	return file_info;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static bool is_elf_file(Elf64_Ehdr* elf_header) {
	return (elf_header->e_ident[EI_MAG0] == ELFMAG0 || elf_header->e_ident[EI_MAG1] == ELFMAG1 ||
			elf_header->e_ident[EI_MAG2] == ELFMAG2 || elf_header->e_ident[EI_MAG3] == ELFMAG3);
}

static Elf64_Phdr* get_program_headers(Elf64_Ehdr* elf_header, struct vfs_file* elf_file) {
	Elf64_Phdr* program_headers = kmalloc(sizeof(Elf64_Phdr) * elf_header->e_phnum);

	vfs_seek(elf_file, (int64_t)elf_header->e_phoff, SKW_VFS_SEEK_SET);
	vfs_read(elf_file, (uint8_t*)program_headers, sizeof(Elf64_Phdr) * elf_header->e_phnum);
	return program_headers;
}

static uint64_t find_heap_start(Elf64_Phdr* program_headers, size_t ph_count) {
	uint64_t heap_start = 0;
	for (uint64_t i = 0; i < ph_count; i++) {
		Elf64_Phdr* curr_ph = &program_headers[i];
		if (curr_ph->p_type != PT_LOAD) {
			continue;
		}

		uint64_t segment_end_vaddr = curr_ph->p_vaddr + curr_ph->p_memsz;
		if (segment_end_vaddr > heap_start) {
			heap_start = segment_end_vaddr;
		}
	}

	KERNEL_ASSERT(heap_start != 0, "Could not found where the process heap should start");

	return ALIGN_UP(heap_start, REGULAR_PAGE_SIZE);
}

static void load_binary_segments(Elf64_Phdr* program_headers, size_t ph_count,
								 struct vfs_file* elf_file, void* pagemap_ptr) {
	for (uint32_t i = 0; i < ph_count; i++) {
		Elf64_Phdr* curr_ph = &program_headers[i];
		if (curr_ph->p_type == PT_LOAD) {
			load_segment(curr_ph, elf_file, pagemap_ptr);
		}
	}
}

static void load_segment(Elf64_Phdr* program_header, struct vfs_file* elf_file, void* pagemap_ptr) {
	// Mapping the segment to memory:
	uint64_t segment_page_count = get_elf_segment_pages_count(program_header);

	uint8_t* segment_vaddr = (uint8_t*)program_header->p_vaddr;
	uint8_t* phys_pages = pmm_alloc_pages(segment_page_count);
	for (uint64_t i = 0; i < segment_page_count; i++) {
		int map_status = vmm_map_page(pagemap_ptr, segment_vaddr + (i * REGULAR_PAGE_SIZE),
									  phys_pages + (i * REGULAR_PAGE_SIZE),
									  MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
		KERNEL_ASSERT(map_status == 0,
					  "Trying to load an ELF segment which overlaps with another segment.\n This "
					  "feature is not supported");
	}

	// Loading the segment to memory:
	uint8_t* segment_start_hhdm_location =
		(uint8_t*)vmm_phys_to_virt_hhdm(phys_pages) + (program_header->p_vaddr % REGULAR_PAGE_SIZE);
	vfs_seek(elf_file, (int64_t)program_header->p_offset, SKW_VFS_SEEK_SET);
	vfs_read(elf_file, segment_start_hhdm_location, program_header->p_filesz);

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
