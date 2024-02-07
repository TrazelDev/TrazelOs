#include "keyboardInterrupt.h"
#include "src/interrupts/idt.h"
#include "utility/utility.h"

void initKeyboardInterrupt()
{
    // getting the asm isr address:
    extern uint64_t asmIsrKeyboardHandler;
    uint64_t asmIsrKeyboardHandlerAddress = (uint64_t)(&asmIsrKeyboardHandler);


    initSpecificInterrupt(
        asmIsrKeyboardHandlerAddress, NO_INTERRUPT_STACK_TABLE, 
        IDT_TA_InterruptGate, NO_INTERRUPT_EXTRA_BITS, InterruptIndexes::keyboard
    );

    
}