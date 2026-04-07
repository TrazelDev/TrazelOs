#include <include/strings.h>
#include <include/types.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>
#include <kernel/include/vmm.h>

#include "kernel/include/acpi.h"

struct rsdp {
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_address;
} __attribute__((packed));

/* the rsdp struct needs to be equal to zero when all of the bytes are added, this verifies it */
static void verify_rsdp_checksum(const struct rsdp* rsdp);
static void verify_acpi_std_header_checksum(const struct acpi_std_header* std_header);
static void print_xsdp(const struct rsdp* rsdp);
static void print_acpi_sdt_header(const struct acpi_std_header* header);

static bool g_acpi_init = false;
struct acpi_std_header* g_rsdt = NULL;
void init_acpi(volatile struct limine_rsdp_response* rsdp_response) {
	if (g_acpi_init) {
		return;
	}
	g_acpi_init = true;

	KERNEL_ASSERT(rsdp_response != NULL, "RSDP response is NULL bootloader did not provide it");
	KERNEL_ASSERT(rsdp_response->address != 0, "RSDP address is 0 bootloader did not provide it");

	struct rsdp* rsdp = (struct rsdp*)rsdp_response->address;
	if (rsdp->revision > 0) {
		KERNEL_PANIC(
			"OS only supports version 1 of ACPI, but the system uses version 2\n"
			"You can easily make it work if you need to by using simple conversion");
	}

	verify_rsdp_checksum(rsdp);

	g_rsdt =
		(struct acpi_std_header*)vmm_phys_to_virt_hhdm((void*)((uint64_t)(rsdp->rsdt_address)));
	printk("Initialized ACPI successfully! RSDT address: %x\n", g_rsdt);
}

struct acpi_std_header* acpi_get_std_header(enum acpi_table_type type) {
	const char ATT_TO_SIGNATURE_MAP[ATT_TABLE_SIZE][4] = {"APIC"};
	const char* signature = ATT_TO_SIGNATURE_MAP[type];

	uint64_t rsdt_entries_count =
		(g_rsdt->length - sizeof(struct acpi_std_header)) / sizeof(uint32_t);
	uint32_t* rsdt_entries = (uint32_t*)((uint64_t)g_rsdt + sizeof(struct acpi_std_header));

	for (uint64_t i = 0; i < rsdt_entries_count; i++) {
		struct acpi_std_header* acpi_header =
			(struct acpi_std_header*)vmm_phys_to_virt_hhdm((void*)(uint64_t)rsdt_entries[i]);

		if (strncmp(signature, acpi_header->signature, 4) != 0) {
			continue;
		}

		verify_acpi_std_header_checksum(acpi_header);
		return acpi_header;
	}

	return NULL;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static void verify_rsdp_checksum(const struct rsdp* rsdp) {
	int8_t* ptr = (int8_t*)rsdp;
	int32_t count = 0;
	for (uint64_t i = 0; i < sizeof(struct rsdp); i++) {
		count += ptr[i];
	}

	if (count % 0x100 != 0) {
		printk("The checksum is %d", count);
		KERNEL_PANIC("RSDP checksum is invalid");
	}
}
static void verify_acpi_std_header_checksum(const struct acpi_std_header* std_header) {
	int8_t* ptr = (int8_t*)std_header;
	int32_t count = 0;
	for (uint64_t i = 0; i < std_header->length; i++) {
		count += ptr[i];
	}

	if (count % 0x100 != 0) {
		printk("The checksum is %d", count);
		KERNEL_PANIC("ACPI SDT header checksum is invalid");
	}
}

static void print_xsdp(const struct rsdp* rsdp) {
	printk("XSDP_t structure:\n");
	printk("Signature:        %s\n", rsdp->signature);
	printk("Checksum:         %u\n", rsdp->checksum);
	printk("OEMID:            %s\n", rsdp->oemid);
	printk("Revision:         %u\n", rsdp->revision);
	printk("RsdtAddress:      %x\n", rsdp->rsdt_address);
}
static void print_acpi_sdt_header(const struct acpi_std_header* header) {
	printk("ACPISDTHeader structure:\n");
	printk("  Signature:       %s\n", header->signature);
	printk("  Length:          %u\n", header->length);
	printk("  Revision:        %u\n", header->revision);
	printk("  Checksum:        %u\n", header->checksum);
	printk("  OEMID:           %s\n", header->oemid);
	printk("  OEMTableID:      %s\n", header->oem_table_id);
	printk("  OEMRevision:     %x\n", header->oem_revision);
	printk("  CreatorID:       %x\n", header->creator_id);
	printk("  CreatorRevision: %x\n", header->creator_revision);
}
