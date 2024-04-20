#include "virtualMemoryManager.h"
#include "src/memory/pageRequestor.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"

// functions for use outside:
PhysicalAddress getPML4Address()
{
    PhysicalAddress addr; 
    asm volatile("movq %%cr3, %0" : "=r"(addr.raw));
    return addr;
}
PhysicalAddress getPhysicalAddress(VirtualAddress vAddr, PhysicalAddress plm4Address)
{
    // getting the page index of the first table the pml4 table:
    uint64_t pageIndex = plm4Address.addr.pageIndex;
    // an array that has all entree indexes of every page table:
    uint64_t pageTableEntriesIndexes[PAGE_TABLE_MAX_LEVEL] = { vAddr.PML4_INDEX, vAddr.PDPI_INDEX, vAddr.PD_INDEX, vAddr.PT_INDEX }; 
    

    // going in a loop of PAGE_TABLE_MAX_LEVEL times to find the page index of the page the virtual address points to: 
    for(uint64_t i = 0; i < PAGE_TABLE_MAX_LEVEL; i++)
    {
        pageIndex = getNextPageIndex(pageIndex, pageTableEntriesIndexes[i]);  
    }


    // getting the physical address of the page we need:
    PhysicalAddress pAddress;
    pAddress.addr.pageIndex = pageIndex;
    pAddress.addr.offset    = vAddr.VIRTUAL_ADDRESS_OFFSET;
    return pAddress;
}
bool mapMemory(PhysicalAddress pAddr, VirtualAddress vAddr, bool overrunTableEntry, PhysicalAddress plm4Address)
{
    // getting the page index of the first table the pml4
    uint64_t pageIndex = plm4Address.addr.pageIndex;
    uint64_t pageTableEntriesIndexes[PAGE_TABLE_MAX_LEVEL] = { vAddr.PML4_INDEX, vAddr.PDPI_INDEX, vAddr.PD_INDEX, vAddr.PT_INDEX }; 
    bool isPageTableIndex = true; // this value says if the entry in the current page table is the page index of another page table or the index of the physical address  

    // looping on all the pages until we reach the index that represents the final page table the actual pt 
    for(uint64_t i = 0; i < PAGE_TABLE_MAX_LEVEL; i++)
    {
        // checking if we have reach the pt and the page index is now pointing to the actual physical page we want to map
        if(i == PAGE_TABLE_MAX_LEVEL - 1) { isPageTableIndex = false; }

        pageIndex = createOrFindNextPageIndex(pageIndex, pageTableEntriesIndexes[i], pAddr.addr.pageIndex, isPageTableIndex, overrunTableEntry);
    }
    
    // invalidating the tlb cashing
    FLUSH_BUFFER(vAddr.raw);

    // returning if the final index in the pt is pointing to the desired page that the physical address is in and the mapping succeeded: 
    return (pageIndex == pAddr.addr.pageIndex);
}
void unmapMemory(VirtualAddress vAddr)
{
    uint64_t pageIndex = getPML4Address().addr.pageIndex;
    uint64_t pageTableEntriesIndexes[PAGE_TABLE_MAX_LEVEL - 1] = { vAddr.PML4_INDEX, vAddr.PDPI_INDEX, vAddr.PD_INDEX };
    bool isPageTableIndex = true;

    for(uint64_t i = 0; i < PAGE_TABLE_MAX_LEVEL - 1; i++)
    {
        pageIndex = createOrFindNextPageIndex(pageIndex, pageTableEntriesIndexes[i], NULL, isPageTableIndex, false);
    }
    
    PageTable* pageTable = getPageTable(pageIndex);
    pageTable->entries[vAddr.PT_INDEX].attributes.index   = 0;
    pageTable->entries[vAddr.PT_INDEX].attributes.present = false;

    FLUSH_BUFFER(vAddr.raw);
}

// internal utility functions:
static inline uint64_t createOrFindNextPageIndex(uint64_t pageIndex, uint64_t entryIndex, 
    uint64_t physicalPageIndex, bool isPageTableIndex, bool overrunTableEntry)
{
    // getting the page table from the page index
    PageTable* pageTable = getPageTable(pageIndex);

    if(pageTable->entries[entryIndex].attributes.present == false ||
        // making sure that this is the last entry in the actual pt and we can overrun the current physical address for this virtual address:
        (!isPageTableIndex && overrunTableEntry)) 
    {
        setNewPageEntry(pageTable, entryIndex, isPageTableIndex, physicalPageIndex);
    }

    pageTable->entries[entryIndex].attributes.present  = true;
    pageTable->entries[entryIndex].attributes.writable = true;
    return pageTable->entries[entryIndex].attributes.index;
}
static inline void setNewPageEntry(PageTable* currentPageTable, uint64_t entryIndex, bool isPageTableIndex, uint64_t physicalPageIndex)
{
    // checking if the next page has to be an identity mapped page because this is a page that is going to be used for 
    // a page table or we should use the index that is being provided because we map it to the requested physical address:
    PhysicalAddress pageAddress;


    if(isPageTableIndex) 
    { 
        pageAddress                = requestIdentityMappedPage();
        if(!pageAddress.raw)
        {
            //createPhysicalAddr(requestPages(1).raw);
        }

        ASSERT_PRINT_ERROR(pageAddress.raw != NULL,
            printf("Error: there are no more identity mapped pages and the page table couldn't have been created\n"))
    }
    else pageAddress.addr.pageIndex = physicalPageIndex;


    
    
    currentPageTable->entries[entryIndex].attributes.index = pageAddress.addr.pageIndex; // setting the index of the next page table
}
static inline PageTable* getPageTable(uint64_t pageIndex)
{
    // making sure that the page index is not NULL:
    ASSERT_PRINT_ERROR(pageIndex != NULL, 
        printf("Error: page table index points to NULL\n"))


    uint64_t pageTableAddress = pageIndex * PAGE_SIZE;
    ASSERT_PRINT_ERROR(pageTableAddress > MAX_VIRTUAL_ADDRESS_MAPPED_TO_PHYSICAL, 
        printf("Error: physical and virtual address are different for this page therefor there is no way to access to this page table\n"))      
    

    return (PageTable*)(pageTableAddress); // finding the table it self from the page index
}
inline uint64_t getNextPageIndex(uint64_t pageIndex, uint64_t entryIndex)
{
    PageTable* pageTable = getPageTable(pageIndex);


    // making sure that the output index of the page is even a page that is present in the system:
    ASSERT_PRINT_ERROR(pageTable->entries[entryIndex].attributes.present != NULL,
        printf("Error: the index of the requested page is not present in the system\n"))

    
    return pageTable->entries[entryIndex].attributes.index; // extracting the index of the next table
}

