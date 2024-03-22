#include "idt.h"
#include "specificInterrupts/allInterrupts.h"

static InterruptDescriptor interruptTable[256]; // the interrupt descriptor table
static IdtRegister         idtRegister;         // the idt register that is loaded

void initSpecificInterrupt(uint64_t isrAddress, uint8_t interruptStackTable, 
    uint8_t typesAndAttributes, uint8_t extraBits, uint8_t interruptIndex)
{
    setIsrFunctionAddress(&interruptTable[interruptIndex], isrAddress); // setting the function that will be called when the interrupt is triggered
    interruptTable[interruptIndex].selector.setSelector();              // setting privilege levels and the space of the interrupt
    interruptTable[interruptIndex].ist = interruptStackTable;
    interruptTable[interruptIndex].typesAttr = typesAndAttributes;
    interruptTable[interruptIndex].extraBits  = extraBits;              // extra bits that can store specific information and in the future flags
}


static void loadIDTRegister()
{
    asm volatile(
        "lidt %0 \n\t"  // loading the interrupt register
        "sti     \n\t"  // enabling interrupts
        : 
        : "m"(idtRegister)
        : "%eax", "%ecx", "%edx"
    );
}


void initIDT()
{
    idtRegister.size = 0x1000; 
    idtRegister.idtAddress = (uint64_t)(interruptTable);

    // remapping the programable interrupts controllers ( farther explanation in the function itself )  
    remapPics();

    // initializing all of the current interrupt that the os supports right now:
    initAllInterrupt();

    loadIDTRegister();

    DEBUG_LOG("Initialized the IDT\n");
}