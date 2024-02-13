#include "pageFrameAllocator.h"
#include "memoryMap.h"

static BitMap pageBitMap;
extern uint64_t kernelSectorsLoaded;

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
    ASSERT_PRINT_ERROR(condition, printf("Error"));

    printf("sectors loaded: %d\n", AMOUNT_OF_SECTORS_KERNEL_TAKES);
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

