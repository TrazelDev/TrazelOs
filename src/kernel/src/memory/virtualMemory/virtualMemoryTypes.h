#pragma once
#include "utility/utility.h"
#include "src/memory/physicalMemory/pageFrameAllocatorDefs.h"


#define PAGE_TABLE_MAX_LEVEL 4
#define PAGING_LEVEL_MAX_PAGE_COUNT (512ULL * 512 * 512 * 512)
#define VIRTUAL_ADDRESS_OFFSET addr.offset
#define PT_INDEX               addr.ptIndex
#define PD_INDEX               addr.pdIndex
#define PDPI_INDEX             addr.pdpIndex
#define PML4_INDEX             addr.plm4Index
#define SIGN_EXTENSION_BITMASK addr.signExtension
typedef union
{
    struct 
    {
        uint64_t offset              : 12; 
        uint64_t ptIndex             : 9 ;
        uint64_t pdIndex             : 9 ;
        uint64_t pdpIndex            : 9 ;
        uint64_t plm4Index           : 9 ;
        uint64_t signExtension       : 16; 
    } addr;

    uint64_t   raw;
    uint8_t*   uint8Ptr;
    uint16_t*  uint16Ptr;
    uint32_t*  uint32Ptr;
    uint64_t*  uint64Ptr;
    void*      voidPtr;
} __attribute__((packed)) VirtualAddress;
// __attribute__((packed)) is used to tell the compiler that the members in the struct need to be packed tightly with no aliment because the structure should
// be aligned byte by byte



typedef union
{
    struct
    {
        uint64_t present          : 1;  // page present in memory
        uint64_t writable         : 1;  // read-only if clear, read-write if set
        uint64_t user_access      : 1;  // supervisor level only if clear
        uint64_t write_through    : 1;  // write-through caching enabled if set
        uint64_t cache_disable    : 1;  // cache disabled if set
        uint64_t accessed         : 1;  // page accessed since last refresh
        uint64_t dirty            : 1;  // page written to since last refresh

        // page table with the size (512 * 0x1000 = 2048KB = 2MB ) a typical page is only the size 4KB but this page is special and
        // and it can hold 512 page tables ( 1 page - for table )
        uint64_t huge_page        : 1;  
        uint64_t global           : 1;  // if set, prevents TLB from updating
        uint64_t reserved         : 3;  // reserved bits
        uint64_t index            : 40; // index of entry in the pages of the memory
        uint64_t avl2             : 7;  
        uint64_t pk               : 4;
        uint64_t xd               : 1;
    } __attribute__((packed)) attributes;

    uint64_t raw;
} __attribute__((packed)) PageTableEntry    ;
typedef PageTableEntry    PageDirectoryEntry;
typedef PageTableEntry    PagePDPTEntry     ;
typedef PageTableEntry    PagePML4Entry     ;

inline VirtualAddress createVirtualAddr(void* ptr)
{
    VirtualAddress vAddr;
    vAddr.voidPtr = ptr;
    return vAddr;
}
inline VirtualAddress createVirtualAddr(uint64_t addr)
{
    VirtualAddress vAddr;
    vAddr.raw = addr;
    return vAddr;
}


// page table is a generic way to call the page table, page directory
#define PAGE_TABLE_ENTRIES_IN_ONE_PAGE 512
typedef struct
{
    PageTableEntry entries[PAGE_TABLE_ENTRIES_IN_ONE_PAGE];
} __attribute__((packed, aligned(PAGE_SIZE))) PageTable;
typedef PageTable PageDirectoryTable;
typedef PageTable PagePDPTTable;
typedef PageTable PagePML4Table;