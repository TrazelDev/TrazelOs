#include "memoryMap.h"
#include "utility/print/printVga.h"


// the amount of regions and the regions data itself:
uint8_t usableMemoryRegionCount    = 0;
uint8_t reservedMemoryRegionsCount = 0;
MemoryMapEntry* usableMemoryRegions[MAX_AMOUNT_OF_REGIONS_IN_MEMORY];  
MemoryMapEntry* reservedMemoryRegions[MAX_AMOUNT_OF_REGIONS_IN_MEMORY];


extern uint8_t memoryRegionCount;
extern uint64_t addressWithMemoryMaps;


// functions for getting specific memory regions:
uint8_t getCertainMemoryRegionCount(MemoryRegion memoryRegion)
{
    if(memoryRegion == MemoryRegion::UsableRAM ) { return usableMemoryRegionCount   ; }
    if(memoryRegion == MemoryRegion::Reserved  ) { return reservedMemoryRegionsCount; }

    ASSERT_PRINT_ERROR(true, printf("Error: the choosen memory region is not supported by this function\n")) 
    return 0;
}
MemoryMapEntry** getCertainMemoryRegions(MemoryRegion memoryRegion)
{   
    // returning the wanted regions if they are already detected: 
    MemoryMapEntry** memoryRegions = alreadyCalculatedRegions(memoryRegion);
    if(memoryRegions) { return memoryRegions; }

    // pointing to the right array and the right counter
    memoryRegions = (memoryRegion == MemoryRegion::UsableRAM) ?  usableMemoryRegions      : reservedMemoryRegions     ;
    uint8_t* wantedRegionCount = (memoryRegion == MemoryRegion::UsableRAM) ? &usableMemoryRegionCount : &reservedMemoryRegionsCount;

    
    // going in a loop and storing the wanted regions in right arrays:
    MemoryMapEntry* memMap;
    for (uint8_t i = 0; i < memoryRegionCount && i < MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS; i++)
    {
        memMap = (MemoryMapEntry*)addressWithMemoryMaps + i;

        // checking if the current region is type 1 ( usable ram region ):
        if(memMap->regionType == memoryRegion)
        {
            memoryRegions[(*wantedRegionCount)] = memMap;   // setting the region in the array
            (*wantedRegionCount)++;     // increasing the amount of regions whatever was choosen

            // checking if there are too many regions to fit in the array
            ASSERT_PRINT_ERROR((!((*wantedRegionCount) + 1 == MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS)), 
                printf("Error: too many regions and not enough place to store all regions\n"))
        }
    }

    
    return memoryRegions;
}
static MemoryMapEntry** alreadyCalculatedRegions(MemoryRegion memoryRegion)
{
    // making sure that there are no problems with the way the code is used:
    ASSERT_PRINT_ERROR(memoryRegionCount > MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS, 
        printf("Error: the amount of regions is bigger then the amount of regions that can fit in the array\n"));
    ASSERT_PRINT_ERROR(memoryRegion == MemoryRegion::Reserved && memoryRegion == MemoryRegion::UsableRAM, 
        printf("Error: function only calculates reserved and free memory regions\n"));

    static bool alreadyCalculatedFreeMemoryRegions     = false;
    static bool alreadyCalculatedReservedMemoryRegions = false;

    // returning the wanted memory regions if they are already calculated:
    if(memoryRegion == MemoryRegion::UsableRAM && alreadyCalculatedFreeMemoryRegions    ) { return usableMemoryRegions  ; }
    if(memoryRegion == MemoryRegion::Reserved  && alreadyCalculatedReservedMemoryRegions) { return reservedMemoryRegions; }

    // returning nullptr in the case that the region has to be calculated:
    return nullptr;
}


// functions for getting all of the memory regions:
MemoryMapEntry* getAllMemoryRegions()
{
    return (MemoryMapEntry*)addressWithMemoryMaps;
}
uint8_t getMemoryRegionCount()
{
    return memoryRegionCount;
}


// a way to get the size of all the memory:
uint64_t getMemorySize()
{
    static uint64_t memorySizeBytes = 0;
    if(memorySizeBytes != 0) { return memorySizeBytes; };

    // getting all of the regions:
    MemoryMapEntry* freeMemoryMapEntries = getAllMemoryRegions();

    // counting the amount of memory whether it is usable or not:
    for(int i = 0; i < memoryRegionCount; i++)
    {
        memorySizeBytes += freeMemoryMapEntries[i].regionLength;
    }


    return memorySizeBytes;
}


// a way to get the memory map of the biggest chunk of memory that can be used freely:
MemoryMapEntry* getFreeMemoryBiggestEntry()
{
    MemoryMapEntry** entries = getCertainMemoryRegions(MemoryRegion::UsableRAM);
    MemoryMapEntry* biggestSizeEntry = entries[0];

    // checking what is the address of region with the most memory:
    for(int i = 1; i < usableMemoryRegionCount; i++)
    {
        if(entries[i]->regionLength > biggestSizeEntry->regionLength)
        {
            biggestSizeEntry = entries[i];
        }
    }

    return biggestSizeEntry;
}


#ifdef DEBUG

void printAllRegions()
{
    uint64_t count = 0;
    uint64_t countUsable = 0;
    MemoryMapEntry* allMemoryRegions = (MemoryMapEntry*)addressWithMemoryMaps;

    for(int i = 0; i < memoryRegionCount; i++)
    {
        // printing the specific memory region:
        printf("region number %d:\n", i);
        printMemoryMap(&(allMemoryRegions[i]));

        if(allMemoryRegions[i].regionType == MemoryRegion::UsableRAM) { countUsable += allMemoryRegions[i].regionLength; }
        else                                                          { count       += allMemoryRegions[i].regionLength; }
    }

    // printing the amount of memory each region has:
    printf("Non usable ram: %x\n", count);
    printf("Usable ram    : %x\n\n", countUsable);
}
void printMemoryMap(MemoryMapEntry* memoryMap)
{
    printf("base addr: %x, ", memoryMap->baseAddress);
    printf("region length: %x, ", memoryMap->regionLength);
    printf("type: %d, ", memoryMap->regionType);
    printf("attributes: %d\n\n", memoryMap->extendedAttributes);
}

#endif // DEBUG