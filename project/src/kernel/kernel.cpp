#include "tests/test.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "src/processManagement/processesFunctions.h"




extern "C" void _start()
{
    initPrint();
    initPageFrameAllocator();
    initIDT();
    initMemoryPool();

    IF_DEBUG(testEverything());
    processesInSystem[1].func();
}