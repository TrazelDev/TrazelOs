#include "idt.h"


static InterruptDescriptor interruptTable[256];
static IdtRegister         idtRegister;

void initSpecificInterrupt(uint64_t isrAddress, uint8_t interruptStackTable, 
    uint8_t typesAndAttributes, uint8_t extraBits, uint8_t interruptIndex)
{
    setIsrFunctionAddress(&interruptTable[interruptIndex], isrAddress); // setting the function that will be called when the interrupt is triggered
    interruptTable[interruptIndex].selector.setSelector();              // setting privilege levels and the space of the interrupt
    interruptTable[interruptIndex].ist = interruptStackTable;
    interruptTable[interruptIndex].typesAttr = typesAndAttributes;
    interruptTable[interruptIndex].extraBits  = extraBits;              // extra bits that can store specific information and in the future flags
}

void initIDT()
{
    idtRegister.size = 0x1000; 
    idtRegister.idtAddress = (uint64_t)interruptTable;

    // loading all of the interrupts
    

    remapPics();

}