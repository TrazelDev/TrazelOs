#include "pageFault.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"


void initPageFaultInterrupt()
{
    extern uint64_t asmIsrPageFaultHandler;
    uint64_t asmIsrPageFaultHandlerAddress = (uint64_t)(&asmIsrPageFaultHandler);

    /* Example for the keyboard hardware interrupt 
    
    initSpecificInterrupt(
        asmIsrKeyboardHandlerAddress, NO_INTERRUPT_STACK_TABLE, 
        IDT_TA_InterruptGate, NO_INTERRUPT_EXTRA_BITS, (uint8_t)InterruptIndexes::keyboard
    );

    enableHardwareInterrupt(IRQ::keyboard); // only for hardware interrupts
    
    */
}


void isrPageFaultHandler()
{
    // if it is hardware interrupt do not forget:
    // picSendEOI(IRQ::thisHardwareInterrupt);  
}
