#include <drivers/vga_text.h>
#include <include/elf.h>
#include <include/integer_utils.h>
#include <include/mem_utils.h>
#include <include/types.h>

#include "kernel_elf_loader.h"

static void print_elf64_header(const Elf64_Ehdr* ehdr);
static void print_elf64_program_header(Elf64_Phdr* phdr, uint32_t index);
static void load_binary_segments(const Elf64_Ehdr* elf_header, Elf64_Phdr* program_headers);
static void load_segment(Elf64_Phdr* program_header, uint8_t* kernel_elf_data);

int load_and_jump_kernel(void* kernel_elf_data, size_t kernel_elf_size) {
	const Elf64_Ehdr* elf_header = (const Elf64_Ehdr*)kernel_elf_data;
	Elf64_Phdr* program_header = (Elf64_Phdr*)(kernel_elf_data + elf_header->e_phoff);
	load_binary_segments(elf_header, program_header);

	int (*kmain)() = (int (*)())(elf_header->e_entry);
	return kmain();
}

static void load_binary_segments(const Elf64_Ehdr* elf_header, Elf64_Phdr* program_headers) {
	Elf64_Phdr* curr_ph = program_headers;
	for (uint32_t i = 0; i < elf_header->e_phnum; i++) {
		curr_ph = &program_headers[i];
		if (curr_ph->p_type == PT_LOAD) {
			load_segment(curr_ph, (uint8_t*)elf_header);
		}
	}
}

static void load_segment(Elf64_Phdr* program_header, uint8_t* kernel_elf_data) {
	uint8_t* segment_vma = (uint8_t*)program_header->p_vaddr;
	uint8_t* segment_location = (uint8_t*)(kernel_elf_data + program_header->p_offset);

	uint64_t file_size = program_header->p_filesz;
	uint64_t memory_size = program_header->p_memsz;

	memcpy(segment_vma, segment_location, file_size);
	if (memory_size > file_size) {
		memset(segment_vma + file_size, 0, memory_size - file_size);
	}
}

void print_elf64_header(const Elf64_Ehdr* ehdr) {
	// 64 bytes is safely large enough for a 64-bit int converted to binary,
	// so it is more than enough for hex and decimal strings.
	char buf[64];

	print_string("--- ELF64 Header ---\n");

	// 1. Magic Bytes (Should be 0x7F, 0x45 ('E'), 0x4C ('L'), 0x46 ('F'))
	print_string("Magic: ");
	for (int i = 0; i < 4; i++) {
		print_string(itoa_unsigned(ehdr->e_ident[i], buf, HEX));
		print_string(" ");
	}
	print_string("\n");

	// 2. Class and Data (1 = 32-bit, 2 = 64-bit)
	print_string("Class: ");
	print_string(itoa_unsigned(ehdr->e_ident[EI_CLASS], buf, DECIMAL));
	print_string("\n");

	// 3. Object Type and Machine Architecture
	print_string("Type: 0x");
	print_string(itoa_unsigned(ehdr->e_type, buf, HEX));
	print_string("\n");

	print_string("Machine: 0x");
	print_string(itoa_unsigned(ehdr->e_machine, buf, HEX));
	print_string("\n");

	// 4. Memory Addresses and Offsets
	print_string("Entry Point: 0x");
	print_string(itoa_unsigned(ehdr->e_entry, buf, HEX));
	print_string("\n");

	print_string("Program Header Offset: 0x");
	print_string(itoa_unsigned(ehdr->e_phoff, buf, HEX));
	print_string("\n");

	print_string("Section Header Offset: 0x");
	print_string(itoa_unsigned(ehdr->e_shoff, buf, HEX));
	print_string("\n");

	// 5. Sizes and Counts (Usually easier to read in Decimal)
	print_string("ELF Header Size: ");
	print_string(itoa_unsigned(ehdr->e_ehsize, buf, DECIMAL));
	print_string(" bytes\n");

	print_string("Program Header Entry Size: ");
	print_string(itoa_unsigned(ehdr->e_phentsize, buf, DECIMAL));
	print_string(" bytes\n");

	print_string("Program Header Count: ");
	print_string(itoa_unsigned(ehdr->e_phnum, buf, DECIMAL));
	print_string("\n");

	print_string("Section Header Entry Size: ");
	print_string(itoa_unsigned(ehdr->e_shentsize, buf, DECIMAL));
	print_string(" bytes\n");

	print_string("Section Header Count: ");
	print_string(itoa_unsigned(ehdr->e_shnum, buf, DECIMAL));
	print_string("\n");

	print_string("String Table Index: ");
	print_string(itoa_unsigned(ehdr->e_shstrndx, buf, DECIMAL));
	print_string("\n--------------------\n");
}

#include <elf.h>

void print_elf64_program_header(Elf64_Phdr* phdr, uint32_t index) {
	char buf[64];

	print_string("--- Program Header [");
	print_string(itoa_unsigned(index, buf, DECIMAL));
	print_string("] ---\n");

	// 1. Type (1 = PT_LOAD, the only one we care about for booting)
	print_string("Type: 0x");
	print_string(itoa_unsigned(phdr->p_type, buf, HEX));
	if (phdr->p_type == PT_LOAD) {
		print_string(" (PT_LOAD)");
	}
	print_string("\n");

	// 2. Memory Addresses (The most important part right now!)
	print_string("VMA (Exec Addr): 0x");
	print_string(itoa_unsigned(phdr->p_vaddr, buf, HEX));
	print_string("\n");

	print_string("LMA (Load Addr): 0x");
	print_string(itoa_unsigned(phdr->p_paddr, buf, HEX));
	print_string("\n");

	// 3. File Location and Sizes
	print_string("Offset in File: 0x");
	print_string(itoa_unsigned(phdr->p_offset, buf, HEX));
	print_string("\n");

	print_string("Size in File: ");
	print_string(itoa_unsigned(phdr->p_filesz, buf, DECIMAL));
	print_string(" bytes\n");

	print_string("Size in RAM:  ");
	print_string(itoa_unsigned(phdr->p_memsz, buf, DECIMAL));
	print_string(" bytes\n");

	// 4. Flags and Alignment
	print_string("Flags: 0x");
	print_string(itoa_unsigned(phdr->p_flags, buf, HEX));
	print_string("\n");

	print_string("Alignment: 0x");
	print_string(itoa_unsigned(phdr->p_align, buf, HEX));
	print_string("\n------------------------\n");
}
