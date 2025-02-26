#pragma once
#include "memoryMapTypes.h"


/// @param memoryRegion the type of the region that we would like to get its array size
/// @return the size of the array that the function getCertainMemoryRegions() returns
uint8_t getCertainMemoryRegionCount(MemoryRegion memoryRegion);


/// @brief gets a type of memory region we would like to to get array of and returns an array of pointer to the regions of this type
/// supports MemoryRegion::UsableRAM and MemoryRegion::Reserved, also the amount of regions will be stored in the 
/// values usableMemoryRegionCount and reservedMemoryRegionsCount respectively
/// @param memoryRegion the type of the region we would like to get an array of
/// @return the array of MemoryMapEntry pointers to the type of region we want. the size of the array can be extracted from the function getCertainMemoryRegionCount()
MemoryMapEntry** getCertainMemoryRegions(MemoryRegion memoryRegion);


/// @brief the function is checking if this specific regions has already been calculated and if does then it will return it.
/// this is a helper functions for getCertainMemoryRegions
/// @return the regions if it is already calculated and nullptr if not
static MemoryMapEntry** alreadyCalculatedRegions(MemoryRegion memoryRegion);


/// @brief use the value memoryRegionCount in order to know how many elements are there in the array that is returned
/// @return an array of MemoryMapEntry which has the size that can be brought from the function getMemoryRegionCount() 
MemoryMapEntry* getAllMemoryRegions();
/// @return the size of the array from the function getAllMemoryRegions()
uint8_t getMemoryRegionCount();


/// @return the accumulative memory size of all the regions ( including ones that there memory is inaccessible ) 
uint64_t getMemorySize();


/// @return pointer to a memory map of the biggest free to use memory section
MemoryMapEntry* getFreeMemoryBiggestEntry();


#ifdef DEBUG
void printMemoryMap(MemoryMapEntry* memoryMap);
void printAllRegions();
#endif