#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/memoryMap.h"
#include "tests/test.h"

extern "C" void _start()
{
    initPrint();
    printAllRegions();
    initIDT();
    
    IF_DEBUG(testEverything());
}