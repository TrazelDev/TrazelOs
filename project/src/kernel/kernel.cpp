#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/memoryMap.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "tests/test.h"
#include "src/filingSystem/ide.h"

extern "C" void _start()
{
    initPrint();
    initPageFrameAllocator();
    initIDT();
    
    IF_DEBUG(testEverything());
}