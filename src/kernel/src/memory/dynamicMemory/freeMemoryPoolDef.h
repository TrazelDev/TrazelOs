#pragma once
#include "utility/utility.h"
#include "src/memory/pageRequestor.h"


typedef struct MemoryBlock
{
    VirtualAddress blockStartingAddress;
    size_t         blockSize;
    MemoryBlock*   next;
} MemoryBlock;

MemoryBlock* createMainPoolBlock(VirtualAddress block, size_t blockSize);
void* getBlockFromMainPool(size_t requestMemorySize);
inline size_t calculatePagesNeededForMemoryChunk(size_t chuckSize);
inline void removeFromMemoryBlockList(MemoryBlock* currBlock);
