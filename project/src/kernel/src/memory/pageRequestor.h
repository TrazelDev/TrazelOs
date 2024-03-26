#pragma once
#include "physicalMemory/pageFrameAllocatorDefs.h"
#include "virtualMemory/virtualMemoryTypes.h"


VirtualAddress requestPages(size_t pageCount);
void returnPages(VirtualAddress vAddr, size_t pageCount);