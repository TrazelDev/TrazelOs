#pragma once
#include "physicalMemory/pageFrameAllocatorDefs.h"
#include "virtualMemory/virtualMemoryTypes.h"


VirtualAddress requestPages(size_t pageCount);
bool returnPages(VirtualAddress, size_t pageCount);
