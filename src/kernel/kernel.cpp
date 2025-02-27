#include "tests/test.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "src/memory/pageRequestor.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "src/processManagement/roundRobin.h"
#include "src/processManagement/processAccessMacros.h"


extern uint64_t* _next_process;
extern "C" void _start()
{
    initPrint();
    initPageFrameAllocator();
    initIDT();
    initMemoryPool();

    contextSwitch(createProcess(GET_PROCESS_SIZE(_next_process), GET_PROCESS_STARTING_ADDRESS(_next_process)));
    uint64_t* printProcess = GET_NEXT_PROCESS(_next_process);
    contextSwitch(createProcess(*printProcess, (uint64_t)(printProcess + 1)));
    IF_DEBUG(testEverything());
}
