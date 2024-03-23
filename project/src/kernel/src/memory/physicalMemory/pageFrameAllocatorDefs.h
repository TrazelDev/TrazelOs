#pragma once
#include "utility/utility.h"

// the first pages in the system are identity mapped:
#define PAGES_WITH_IDENTITY_MAPPING 0x200
// this is last address that have identity mapping:
#define MAX_VIRTUAL_ADDRESS_MAPPED_TO_PHYSICAL 0x1FFFFF


#define PAGE_SIZE   0x1000
#define SECTOR_SIZE 0x200

// this marco returns the amount of memory that a certain length of memory takes in pages:
#define GET_PAGE_COUNT_MEMORY_BYTES_TAKE(memoryLength) ((memoryLength / PAGE_SIZE) + (1 * (memoryLength % PAGE_SIZE > 0)))


// some kernel sizes:
#define KERNEL_FIRST_ADDRESS 0x9000 
#define AMOUNT_OF_SECTORS_KERNEL_TAKES (*((uint16_t*)(0x5000)))
#define AMOUNT_OF_PAGES_KERNEL_TAKES   GET_PAGE_COUNT_MEMORY_BYTES_TAKE((AMOUNT_OF_SECTORS_KERNEL_TAKES * SECTOR_SIZE))


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


/// @brief function that converts a raw address into a physical address struct
/// @param rawAddr the raw address that we need
/// @return PhysicalAddress instance that holds the value of the raw address
inline PhysicalAddress createPhysicalAddr(uint64_t rawAddr)
{
    PhysicalAddress pAddr;
    pAddr.raw = rawAddr;
    return pAddr;
}


// some macros so it will be easy to use the change pages status functions:
#define freePage(addr)                  changePageStatus(addr , true,  &usedMemory               )
#define lockPage(addr)                  changePageStatus(addr , false, &usedMemory               )
#define reservePage(addr)               changePageStatus(addr , false, &reservedMemory           )
#define unreservePage(addr)             changePageStatus(addr , true,  &reservedMemory           )
#define freePages(addr, pageCount)      changePagesStatus(addr, true,  &usedMemory    , pageCount)
#define lockPages(addr, pageCount)      changePagesStatus(addr, false, &usedMemory    , pageCount)
#define reservePages(addr, pageCount)   changePagesStatus(addr, false, &reservedMemory, pageCount)
#define unreservePages(addr, pageCount) changePagesStatus(addr, true,  &reservedMemory, pageCount)