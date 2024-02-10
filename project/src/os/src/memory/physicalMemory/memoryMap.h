#pragma once
#include "memoryMapTypes.h"

/**
 * @param memoryRegion the type of the region that we would like to get its array size
 * @return the size of the array that the function getCertainMemoryRegions() returns
*/
uint8_t getCertainMemoryRegionCount(MemoryRegion memoryRegion);

/**
 * @brief gets a type of memory region we would like to to get array of and returns an array of pointer to the regions of this type
 * supports MemoryRegion::UsableRAM and MemoryRegion::Reserved, also the amount of regions will be stored in the 
 * values usableMemoryRegionCount and reservedMemoryRegionsCount respectively
 * @param memoryRegion the type of the region we would like to get an array of
 * @return the array of MemoryMapEntry pointers to the type of region we want. the size of the array can be extracted from the function getCertainMemoryRegionCount()
*/
MemoryMapEntry** getCertainMemoryRegions(MemoryRegion memoryRegion);

void printMemoryMap(MemoryMapEntry* memoryMap);
void printAllRegions();