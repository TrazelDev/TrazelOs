#include "pageFrameAllocator.h"
#include "memoryMap.h"

static BitMap pageBitMap;
extern uint64_t kernelSectorsLoaded;


// ways to track the memory:
uint64_t freeMemory     = 0;
uint64_t usedMemory     = 0;
uint64_t reservedMemory = 0;


void initPageFrameAllocator()
{
    // making sure that the function is called only once
    static bool initialize = false;
    if(initialize == true) { return; }


    // getting the region with the biggest size and the size of the bit map:
    MemoryMapEntry* biggestSizeEntry = getFreeMemoryBiggestEntry();
    PhysicalAddress addressOfBiggestSizeEntry;
    addressOfBiggestSizeEntry.raw = biggestSizeEntry->baseAddress;
    uint64_t bitMapSize = (getMemorySize() / PAGE_SIZE) + 1; // 1 - because there might extra pages that will need some bits


    // making sure that the final address of the bit map is in that virtual address and physical address are the same:
    bool condition = addressOfBiggestSizeEntry.raw + bitMapSize < MAX_VIRTUAL_ADDRESS_MAPPED_TO_PHYSICAL;
    ASSERT_PRINT_ERROR(condition, printf("Error: the address that bit map was supposed to be is not identity mapped"));

    initPageBitMap(addressOfBiggestSizeEntry, bitMapSize);


    // look the pages that their pages cannot be accessed:
    lookKernelPages();
    lookSpecialDataPages();
    reserveReservedMemoryPages();

    DEBUG_LOG("Initialized the page frame allocator\n");
}
static void initPageBitMap(PhysicalAddress bitmapStartingAddress, uint64_t bitMapSize)
{
    // initializing the bitmap for the pages:
    // creating a bit map that represents all of the pages in the system and if they are taken or not
    // we are doing it by converting the address of the biggest free memory into a bit map 
    // so we can store the bit map dynamically without the stack
    pageBitMap = BitMap(bitmapStartingAddress.uint8Ptr, bitMapSize);
    pageBitMap.resetBitmapValues(bitmapStartingAddress.uint8Ptr, bitMapSize);

    // setting all of the bit map pages to not be occupied:
    memset((uint64_t*)pageBitMap.getBuffer(), NULL, pageBitMap.getBitCount() / BITS_IN_BYTE);
}


static void reserveReservedMemoryPages()
{
    // MemoryMapEntry** getCertainMemoryRegions(MemoryRegion memoryRegion)
    MemoryMapEntry** reservedMemoryRegions = getCertainMemoryRegions(MemoryRegion::Reserved);
    uint64_t reservedMemoryRegionsCount = getCertainMemoryRegionCount(MemoryRegion::Reserved);

    PhysicalAddress memoryRegionsStartingAddress;
    for(uint64_t i = 0; i < reservedMemoryRegionsCount; i++)
    {   
        memoryRegionsStartingAddress = createPhysicalAddr(reservedMemoryRegions[i]->baseAddress);
        reservePages(memoryRegionsStartingAddress, reservedMemoryRegions[i]->regionLength / PAGE_SIZE);
    }
}
static void lookKernelPages()
{
    PhysicalAddress kernelFirstAddr = createPhysicalAddr(KERNEL_FIRST_ADDRESS);
    lockPages(kernelFirstAddr, AMOUNT_OF_PAGES_KERNEL_TAKES);
}
static void lookSpecialDataPages()
{
    PhysicalAddress kernelFirstAddr = createPhysicalAddr(NULL);
    lockPages(kernelFirstAddr, KERNEL_FIRST_ADDRESS / PAGE_SIZE); // looking the pages from null to the kernel
}


static PhysicalAddress getPageWithinRange(uint64_t startingRange, uint64_t endRange)
{
    PhysicalAddress addr = createPhysicalAddr(NULL);

    // looping on the pages to find an empty page
    for(uint64_t i = startingRange; i < endRange; i++)
    {
        if(pageBitMap[i]) { continue; }  // checking if this page is free
        
        addr.raw = i * PAGE_SIZE; // multiply the index by the page size to get the physical address of the page
        lockPage(addr);
        return addr;
    }

    return addr;
}
PhysicalAddress requestUserPage()
{
    return getPageWithinRange(PAGES_WITH_IDENTITY_MAPPING, MAX_UINT64);
}
PhysicalAddress requestIdentityMappedPage()
{
    return getPageWithinRange(NULL, PAGES_WITH_IDENTITY_MAPPING);
}
void returnPhysicalPage(PhysicalAddress pAddr)
{
    freePage(pAddr);
}

static void changePageStatus(PhysicalAddress addr, bool releaseOrGrabPage, uint64_t* lockOrUnreserve)
{
    uint64_t pageIndex = addr.addr.pageIndex;                     // finding out in what page this address in
    if(pageBitMap[pageIndex] == !releaseOrGrabPage ) { return; }  // checking if the bit for the page is already in the desired state


    pageBitMap.setBit(pageIndex, !releaseOrGrabPage);          // setting the bit to the desired state
    freeMemory += BOOL_TO_INT(releaseOrGrabPage) * PAGE_SIZE;  // adding or subtracting the amount of freeMemory based on the input of the function


    // adding or subtracting the amount of reserved memory or used memory based on the inputs of the function:
    *(lockOrUnreserve) += BOOL_TO_INT(!releaseOrGrabPage) * PAGE_SIZE; 
}
static void changePagesStatus(PhysicalAddress addr, bool releaseOrGrabPage, uint64_t* lockOrUnreserve, uint64_t pageCount)
{
    // going in a loop and changing the status of the pages that we want:
    for(uint64_t i = 0; i < pageCount; i++)
    {
        changePageStatus(addr, releaseOrGrabPage, lockOrUnreserve);
        addr.raw += PAGE_SIZE;
    }
}


#ifdef DEBUG

void printInformation()
{
    printf("sectors kernel takes: %x\n", AMOUNT_OF_SECTORS_KERNEL_TAKES);
    printf("pages kernel takes: %x\n", AMOUNT_OF_PAGES_KERNEL_TAKES);
}
void printTakenPages()
{
    for(uint64_t i = 0; i < pageBitMap.getBitCount(); i++)
    {
        if(pageBitMap[i])
        {
            printf("%d,", i);
        }
    }

    printf("\b\n");
}

#endif