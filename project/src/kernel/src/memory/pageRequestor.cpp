#include "pageRequestor.h"
#include "physicalMemory/pageFrameAllocator.h"
#include "virtualMemory/virtualMemoryManager.h"

VirtualAddress mapAndReturnFirstVirtualPageInSequence(VirtualAddress currentVirtualAddress ,uint64_t sequenceCount)
{
    currentVirtualAddress.raw -= (PAGE_SIZE * (sequenceCount - 1)); // all but the first that we want to preserve because it has the correct pAddr
    for(int i = 0; i < sequenceCount - 1; i++)
    {
        mapMemory(requestUserPage(), currentVirtualAddress, true);
        currentVirtualAddress.raw += PAGE_SIZE;
    }
    currentVirtualAddress.raw -= (PAGE_SIZE * sequenceCount);
    return currentVirtualAddress;
}
VirtualAddress unmapFreePagesThatDoNotFit(VirtualAddress currentVirtualAddress, int pageCount)
{
    currentVirtualAddress.raw -= (PAGE_SIZE * pageCount);
    for(int j = 0; j < pageCount + 1; j++)
    {
        unmapMemory(currentVirtualAddress);
        currentVirtualAddress.raw += PAGE_SIZE;
    }
    return currentVirtualAddress;
}
VirtualAddress requestPages(size_t pageCount)
{
    ASSERT_PRINT_ERROR(pageCount > 0, printf("Requested less then 1 page"));
    
    PhysicalAddress pAddr = requestUserPage();
    VirtualAddress  vAddr;
    vAddr.raw = pAddr.raw;
    int currentPageCount = 0; 
    int i = 0;
    int count = 0;
    
    while(true)
    {
        for(i = 0; i < pageCount; i++)
        {
            if(!mapMemory(pAddr, vAddr)) { /*ASSERT_PRINT_ERROR(false, printf("failed on %d\n", i));*/ break; }
            vAddr.raw += PAGE_SIZE;
        }

        if(i == pageCount) { return mapAndReturnFirstVirtualPageInSequence(vAddr, pageCount); }  
        else               { vAddr = unmapFreePagesThatDoNotFit(vAddr, i); }
    }
}

void returnPages(VirtualAddress vAddr, size_t pageCount)
{
    PhysicalAddress pAddr;
    for(size_t i = 0; i < pageCount; i++)
    {
        pAddr = getPhysicalAddress(vAddr);
        returnPhysicalPage(pAddr);
        unmapMemory(vAddr);
        vAddr.raw += PAGE_SIZE;
    }
}