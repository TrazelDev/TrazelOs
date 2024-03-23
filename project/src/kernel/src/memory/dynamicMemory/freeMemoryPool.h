#pragma once
#include "utility/utility.h"
#include "src/memory/virtualMemory/virtualMemoryManager.h"


typedef struct MemoryBlock
{
    VirtualAddress blockStartingAddress;
    size_t         blockSize;
    MemoryBlock*   next;
} MemoryBlock;


void* malloc(size_t blockSize);
void free(void* blockPtr);
