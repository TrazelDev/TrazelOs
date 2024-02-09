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
        IDT_TA_InterruptGate, NO_INTERRUPT_EXTRA_BITS, (uint8_t)InterruptIndexes::keyboard
    );

    //enableHardwareInterrupt(IRQ::keyboard);
    outb(0x21, 0xfd);
	outb(0xa1, 0xff);
}

void isrKeyboardHandler()
{
    printf("hello world\n");
    inb(0x60);
    outb(IO_PORTS::master_pic_command_port, END_OF_INTERRUPT);
}