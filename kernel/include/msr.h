#pragma once
#include <include/types.h>

void init_msr_cpu();

uint64_t msr_get_register(uint32_t msr);
void msr_set_register(uint32_t msr, uint64_t value);

/** Lists a lot of msrs: https://www.sandpile.org/x86/msr.htm */
enum msr_registers {
	MR_EFER_MSR = 0xC0000080,
	MR_STAR_MSR = 0xC0000081,
	MR_LSTAR_MSR = 0xC0000082,
};

/** docmention at the bottom of this page: https://wiki.osdev.org/Model_Specific_Registers */
union EFER_msr_register {
	uint64_t raw;
	struct {
		uint64_t sce : 1;  // system call extension
		uint64_t reserved : 7;
		uint64_t lme : 1;  // long mode enable
		uint64_t reserved2 : 1;
		uint64_t lma : 1;  // long mode active
		uint64_t nxe : 1;  // no execute enable
		uint64_t svme : 1;
		uint64_t lmsle : 1;
		uint64_t ffxsr : 1;
		uint64_t tce : 1;
		uint64_t reserved3 : 48;
	} reg;
} __attribute__((packed));

/** section 6.1 of amd64 volume 2:
 * https://bluewaters.ncsa.illinois.edu/liferay-content/document-library/amd_2_24593.pdf#G12.1054904
 */
union STAR_msr_register {
	uint64_t raw;
	struct {
		uint32_t syscal_32_eip_target : 32;	 // deprecated when using 64 bit long mode
		// ss = syscall_cs_ss_segment + 8, cs = syscall_cs_ss_segment
		uint16_t syscall_cs_ss_segment : 16;
		// ss = syscret_cs_ss_segment + 8, cs = sysret_cs_ss_segment + 16
		uint16_t systet_cs_ss_segment : 16;
	} reg;
} __attribute__((packed));
