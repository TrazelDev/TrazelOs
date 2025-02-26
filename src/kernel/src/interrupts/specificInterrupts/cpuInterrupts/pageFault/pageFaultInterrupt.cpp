#include "pageFaultInterrupt.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/virtualMemory/virtualMemoryManager.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"

void initPageFaultInterrupt()
{
    extern uint64_t asmIsrPageFaultHandler;
    uint64_t asmIsrPageFaultHandlerAddress = (uint64_t)(&asmIsrPageFaultHandler);

    initSpecificInterrupt(
        asmIsrPageFaultHandlerAddress, NO_INTERRUPT_STACK_TABLE,
        IDT_TA_TrapGate, NO_INTERRUPT_EXTRA_BITS, (uint8_t)InterruptIndexes::page_fault
    );
}


void isrPageFaultHandler(PageFaultErrorCode error)
{
    // checking if the problem is that the page is not present and if it does not then we just terminate the process because we only support none present error
    if(error.controlBits.present)
    {
        sysExit(-1);
    }

    VirtualAddress addressThatTriggeredFault;
    asm volatile
    (
        "movq %%cr2, %%rax \n\t"
        "movq %%rax, %0    \n\t" 
        : "=r"(addressThatTriggeredFault.raw)  // where the output is stored
        : 
        : "%rax" // registers that are used this signals the compiler not to use this register anymore
    );

    // mapping the virtual address to a free virtual address 
    PhysicalAddress pAddr = requestUserPage();
    mapMemory(pAddr, addressThatTriggeredFault);
}
