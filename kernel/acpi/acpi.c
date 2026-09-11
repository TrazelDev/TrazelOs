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

struct xsdp {
	struct rsdp rsdp;
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __attribute__((packed));

/* the rsdp struct needs to be equal to zero when all of the bytes are added, this verifies it */
static void verify_rsdp_checksum(const struct rsdp* rsdp);
static void verify_xsdp_checksum(const struct xsdp* xsdp);
static void verify_acpi_std_header_checksum(const struct acpi_std_header* std_header);
static void print_rsdp(const struct rsdp* rsdp);
static void print_acpi_sdt_header(const struct acpi_std_header* header);

static bool g_acpi_init = false;
static struct acpi_std_header* g_rsdt = NULL;
static bool g_is_xsdt = false;
void init_acpi(volatile struct limine_rsdp_response* rsdp_response) {
	if (g_acpi_init) {
		return;
	}
	g_acpi_init = true;

	KERNEL_ASSERT(rsdp_response != NULL, "RSDP response is NULL bootloader did not provide it");
	KERNEL_ASSERT(rsdp_response->address != 0, "RSDP address is 0 bootloader did not provide it");

	struct rsdp* rsdp = (struct rsdp*)rsdp_response->address;
	verify_rsdp_checksum(rsdp);

	if (rsdp->revision >= 2) {
		struct xsdp* xsdp = (struct xsdp*)rsdp;
		verify_xsdp_checksum(xsdp);
		g_rsdt = (struct acpi_std_header*)vmm_phys_to_virt_hhdm((void*)xsdp->xsdt_address);
		g_is_xsdt = true;
	} else {
		g_rsdt =
			(struct acpi_std_header*)vmm_phys_to_virt_hhdm((void*)((uint64_t)(rsdp->rsdt_address)));
	}
	printk("Initialized ACPI successfully! RSDT address: %x\n", g_rsdt);
}

struct acpi_std_header* acpi_get_std_header(enum acpi_table_type type) {
	const char ATT_TO_SIGNATURE_MAP[ATT_TABLE_SIZE][4] = {"APIC"};
	const char* signature = ATT_TO_SIGNATURE_MAP[type];

	// defining both rsdt and xsdt cause we do not know what you use in compile time:
	uint64_t rsdt_entries_count =
		(g_rsdt->length - sizeof(struct acpi_std_header)) / sizeof(uint32_t);
	uint32_t* rsdt_entries = (uint32_t*)((uint64_t)g_rsdt + sizeof(struct acpi_std_header));
	uint64_t xsdt_entries_count =
		(g_rsdt->length - sizeof(struct acpi_std_header)) / sizeof(uint64_t);
	uint64_t* xsdt_entries = (uint64_t*)((uint64_t)g_rsdt + sizeof(struct acpi_std_header));

	uint64_t entries = (g_is_xsdt) ? xsdt_entries_count : rsdt_entries_count;
	for (uint64_t i = 0; i < entries; i++) {
		struct acpi_std_header* acpi_header;
		if (g_is_xsdt) {
			acpi_header = (struct acpi_std_header*)vmm_phys_to_virt_hhdm((void*)xsdt_entries[i]);
		} else {
			acpi_header =
				(struct acpi_std_header*)vmm_phys_to_virt_hhdm((void*)(uint64_t)rsdt_entries[i]);
		}

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
	uint8_t* ptr = (uint8_t*)rsdp;
	uint32_t count = 0;
	for (uint64_t i = 0; i < sizeof(struct rsdp); i++) {
		count += ptr[i];
	}

	if (count & 0xff) {
		printk("The checksum is %d\n", count);
		KERNEL_PANIC("RSDP checksum is invalid");
	}
}

static void verify_xsdp_checksum(const struct xsdp* xsdp) {
	uint8_t* ptr = (uint8_t*)xsdp;
	uint32_t count = 0;
	for (uint64_t i = 0; i < sizeof(struct xsdp); i++) {
		count += ptr[i];
	}

	if (count & 0xff) {
		printk("The checksum is %d\n", count);
		KERNEL_PANIC("XSDP checksum is invalid");
	}
}

static void verify_acpi_std_header_checksum(const struct acpi_std_header* std_header) {
	uint8_t* ptr = (uint8_t*)std_header;
	int32_t count = 0;
	for (uint64_t i = 0; i < std_header->length; i++) {
		count += ptr[i];
	}

	if (count & 0xff) {
		printk("The checksum is %d", count);
		KERNEL_PANIC("ACPI SDT header checksum is invalid");
	}
}

static void print_rsdp(const struct rsdp* rsdp) {
	printk("RSDP structure:\n");
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
