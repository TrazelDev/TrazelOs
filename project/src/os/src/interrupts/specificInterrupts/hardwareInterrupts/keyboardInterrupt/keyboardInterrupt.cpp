#include "keyboardInterrupt.h"
#include "KBScanCodeSets.h"
#include "src/interrupts/idt.h"
#include "utility/utility.h"

void initKeyboardInterrupt()
{
    // getting the asm isr address:
    extern uint64_t asmIsrKeyboardHandler;
    uint64_t asmIsrKeyboardHandlerAddress = (uint64_t)(&asmIsrKeyboardHandler);


    initSpecificInterrupt(
        asmIsrKeyboardHandlerAddress, NO_INTERRUPT_STACK_TABLE, 
        IDT_TA_InterruptGate, NO_INTERRUPT_EXTRA_BITS, (uint8_t)InterruptIndexes::keyboard
    );

    enableHardwareInterrupt(IRQ::keyboard);
}

void isrKeyboardHandler()
{
    uint8_t scanCode = inb(IO_PORTS::keyboard_data_port);
    
    // making sure that the scan code is in the right range:
    if(scanCode > ARRAY_SIZE(scanCodeLookupTable))
    {
        picSendEOI(IRQ::keyboard);
        return;
    }

    // making sure that we support this scan code:
    if(scanCodeLookupTable[scanCode] == 0)
    {
        picSendEOI(IRQ::keyboard);
        return;
    }
    
    // if satisfies the requirements then printing the value that the scan code represents:
    printf("%c", scanCodeLookupTable[scanCode]);
    picSendEOI(IRQ::keyboard);
}