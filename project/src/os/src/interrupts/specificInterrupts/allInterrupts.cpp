// this is an auto generated file

// including all of the interrupts:
#include "allInterrupts.h"
#include "cpuInterrupts/pageFault/pageFaultInterrupt.h"
#include "hardwareInterrupts/keyboardInterrupt/keyboardInterrupt.h"


#include "utility/utility.h"

// function signature to initialize one interrupt:
typedef void (*InterruptInitFunction)();

void initAllInterrupt()
{
    InterruptInitFunction interruptInits[] =
    {
        &initPageFaultInterrupt,
		&initKeyboardInterrupt,
		
    };

    for(uint8_t i = 0; i < ARRAY_SIZE(interruptInits); i++)
    {
        interruptInits[i]();
    }
}
