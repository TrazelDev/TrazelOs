#include "freeMemoryPool.h"
#include "freeMemoryPoolDef.h"
#include "src/memory/pageRequestor.h"

static MemoryBlock* mainPool;
static MemoryBlock* freePool;
static MemoryBlock* freePoolLastNode;

MemoryBlock* createMemoryBlock(VirtualAddress block, size_t blockSize)
{
    if(blockSize < sizeof(MemoryBlock)) { return nullptr; }
    
    MemoryBlock* memoryBlock = (MemoryBlock*)(block.raw + blockSize - sizeof(MemoryBlock));
    memoryBlock->blockSize = blockSize - sizeof(MemoryBlock);
    memoryBlock->blockStartingAddress = block;
    memoryBlock->next = nullptr;

    return memoryBlock;
}
inline size_t calculatePagesNeededForMemoryChunk(size_t chuckSize)
{
    return (chuckSize / PAGE_SIZE) + ((chuckSize % PAGE_SIZE) * 1);
}
inline void removeFromMemoryBlockList(MemoryBlock* currBlock)
{
    currBlock->blockSize = PAGE_SIZE;
    currBlock->blockStartingAddress = requestPages(1);
}
void* getBlockFromMainPool(size_t requestMemorySize)
{
    MemoryBlock* currBlock = mainPool;

    while(currBlock)
    {
        if(currBlock->blockSize > requestMemorySize)
        {
            currBlock->blockSize -= requestMemorySize;
            currBlock->blockStartingAddress.raw += requestMemorySize;
            return (void*)(currBlock->blockStartingAddress.raw);
        }
        if(currBlock->blockSize == requestMemorySize)
        {
            void* requestBlock = (void*)(currBlock->blockStartingAddress.raw);
            removeFromMemoryBlockList(currBlock);
            return requestBlock;
        }

        if(!currBlock->next)
        {
            size_t newBlockSize = calculatePagesNeededForMemoryChunk(requestMemorySize);
            currBlock->next     = createMemoryBlock(requestPages(newBlockSize), newBlockSize * PAGE_SIZE);
        }
        currBlock = currBlock->next;
    }

    ASSERT_PRINT_ERROR(false, printf("Error: there is a problem with the main pool of the free memory pool\n"))
}
void* getBlockFromFreePool(size_t requestMemorySize)
{
    MemoryBlock* currBlock = freePool;

    while(currBlock)
    {
        if(currBlock->blockSize > requestMemorySize)
        {
            currBlock->blockSize -= requestMemorySize;
            currBlock->blockStartingAddress.raw += requestMemorySize;
            return (void*)(currBlock->blockStartingAddress.raw);
        }
        if(currBlock->blockSize == requestMemorySize)
        {
            void* requestBlock = (void*)(currBlock->blockStartingAddress.raw);
            currBlock->blockStartingAddress = createVirtualAddr(getBlockFromMainPool(sizeof(MemoryBlock)));
            currBlock->blockSize = sizeof(MemoryBlock); 
            return requestBlock;
        }

        currBlock = currBlock->next;
    }

    return nullptr;
}

void initMemoryPool()
{
    mainPool = createMemoryBlock(requestPages(1), PAGE_SIZE);
    freePool = nullptr;
    freePoolLastNode = nullptr;
}
void* malloc_x(size_t blockSize)
{
    void* userWantedBlock = getBlockFromFreePool(blockSize);
    if(!userWantedBlock) { userWantedBlock = getBlockFromMainPool(blockSize); }
    
    return userWantedBlock;
}
void free_x(void* blockPtr, size_t blockSize)
{
    MemoryBlock* freePoolNode = createMemoryBlock(createVirtualAddr(blockPtr), blockSize);
    if(!freePoolNode) 
    {
        freePoolNode = (MemoryBlock*)malloc_x(blockSize); 
        freePoolNode->blockSize = blockSize;
        freePoolNode->blockStartingAddress = createVirtualAddr(blockPtr);
        freePoolNode->next = nullptr;
    }

    if(!freePool) { freePool = freePoolNode; }
    freePoolLastNode->next = freePoolNode;
    freePoolLastNode = freePoolLastNode->next;
}
