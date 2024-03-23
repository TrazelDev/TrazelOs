#pragma once 
#include "utility/utility.h"

#define MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS 20
#define MAX_AMOUNT_OF_REGIONS_IN_MEMORY     20
/*
Memory Map Types:
Usable RAM (Type 1):
Description: This type indicates regions of memory that are available for general use. It's typically used for normal system and application operations.
Use: Usable RAM regions are suitable for allocating memory for the operating system kernel and user applications.

Reserved (Type 2):
Description: Reserved regions are portions of memory set aside for specific purposes, often determined by the hardware or firmware.
Use: Reserved areas can include regions reserved by the BIOS, ACPI tables, and other hardware-related data.

ACPI Reclaimable (Type 3):
Description: ACPI (Advanced Configuration and Power Interface) reclaimable regions are memory areas that can be reclaimed or repurposed by the operating system.
Use: These regions might be used by the operating system for dynamic memory allocation.

ACPI Non-volatile (Type 4):
Description: ACPI non-volatile regions are used for storing ACPI data that needs to persist across system reboots.
Use: Data in these regions typically includes system state information that needs to survive power cycles.

Unusable (Type 5):
Description: Unusable regions are memory areas that are not available for use. This could be due to hardware limitations or reserved regions that cannot be repurposed.
Use: These regions are typically avoided for memory
*/
enum MemoryRegion
{
    UsableRAM      = 1,
    Reserved       = 2,
    Reclaimable    = 3,
    NonReclaimable = 4,
    Unusable       = 5,
};

struct MemoryMapEntry
{
    uint64_t baseAddress;
    uint64_t regionLength;
    uint32_t regionType;
    uint32_t extendedAttributes;
};