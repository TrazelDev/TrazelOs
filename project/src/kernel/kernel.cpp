#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "tests/test.h"

extern "C" void _start()
{
    initPrint();
    initPageFrameAllocator();
    initIDT();
    initMemoryPool();

    IF_DEBUG(testEverything());
}