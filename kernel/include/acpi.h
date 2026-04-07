#pragma once
#include <include/vendor/limine.h>

enum acpi_table_type {
	ATT_ACPI_TABLE_MADT = 0,

	ATT_TABLE_SIZE
};

struct acpi_std_header {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemid[6];
	char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
};

void init_acpi(volatile struct limine_rsdp_response* rsdp_response);
struct acpi_std_header* acpi_get_std_header(enum acpi_table_type type);
