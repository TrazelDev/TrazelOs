#include "memoryMap.h"
#include "utility/print/printVga.h"


uint8_t usableMemoryRegionCount    = 0;
uint8_t reservedMemoryRegionsCount = 0;
extern uint8_t memoryRegionCount;

uint8_t getCertainMemoryRegionCount(MemoryRegion memoryRegion)
{
    if(memoryRegion == MemoryRegion::UsableRAM ) { return usableMemoryRegionCount   ; }
    if(memoryRegion == MemoryRegion::Reserved  ) { return reservedMemoryRegionsCount; }

    ASSERT_PRINT_ERROR(true, printf("the choosen memory region is not supported by this function")) 
    return 0;
}
MemoryMapEntry** getCertainMemoryRegions(MemoryRegion memoryRegion)
{
    // defining arrays for usable memory regions and reserved memory regions: 
    static MemoryMapEntry* usableMemoryRegions[MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS];  
    static MemoryMapEntry* reservedMemoryRegions[MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS]; 
    static bool alreadyCalculatedFreeMemoryRegions     = false;
    static bool alreadyCalculatedReservedMemoryRegions = false;

    // returning the wanted memory regions if they are already calculated: 
    if(memoryRegion == MemoryRegion::UsableRAM && alreadyCalculatedFreeMemoryRegions    ) { return usableMemoryRegions  ; }
    if(memoryRegion == MemoryRegion::Reserved  && alreadyCalculatedReservedMemoryRegions) { return reservedMemoryRegions; }
    
    // checking if the wanted region is supported by the function:
    ASSERT_PRINT_ERROR(memoryRegion == MemoryRegion::Reserved && memoryRegion == MemoryRegion::UsableRAM, 
        printf("Error: function only calculates reserved and free memory regions\n"));

    
    // pointing to the right array and the right counter
    MemoryMapEntry** memoryRegions     = (memoryRegion == MemoryRegion::UsableRAM) ?  usableMemoryRegions      : reservedMemoryRegions     ;
    uint8_t*         wantedRegionCount = (memoryRegion == MemoryRegion::UsableRAM) ? &usableMemoryRegionCount : &reservedMemoryRegionsCount;

    
    // going in a loop and storing the wanted regions in right arrays:
    MemoryMapEntry* memMap;
    for (uint8_t i = 0; i < memoryRegionCount && i < MAX_AMOUNT_OF_USABLE_MEMORY_REGIONS; i++)
    {
        memMap = (MemoryMapEntry*)ADDRESS_WITH_MEMORY_MAPS + i;

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

void printAllRegions()
{
    uint64_t count = 0;
    uint64_t countUsable = 0;
    MemoryMapEntry* allMemoryRegions = (MemoryMapEntry*)ADDRESS_WITH_MEMORY_MAPS;

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