#pragma once
#include "utility/utility.h"

// the first pages in the system are identity mapped:
#define PAGE_WITH_IDENTITY_MAPPING 0x200

// this is last address that have identity mapping:
#define MAX_VIRTUAL_ADDRESS_MAPPED_TO_PHYSICAL 0x1FFFFF

#define PAGE_SIZE   0x1000
#define SECTOR_SIZE 0x200
#define AMOUNT_OF_SECTORS_KERNEL_TAKES *((uint16_t*)(0x5000))

typedef union
{
    struct 
    {
        uint64_t offset    : 12;
        uint64_t pageIndex : 52;
    } addr;

    uint64_t        raw;
    uint8_t*        uint8Ptr;
    uint16_t*       uint16Ptr;
    uint32_t*       uint32Ptr;
    uint64_t*       uint64Ptr;
    void*           voidPtr;
} __attribute__((packed)) PhysicalAddress;
// the __attribute__ tells us that the there will no spacing between the different memories of the struct.