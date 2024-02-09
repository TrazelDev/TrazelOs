#include "allInterrupts.h"
#include "utility/utility.h"

// including all of the interrupts:
#include "hardwareInterrupts/keyboardInterrupt/keyboardInterrupt.h"


// function signature to initialize one interrupt:
typedef void (*InterruptInitFunction)();

void initAllInterrupt()
{
    InterruptInitFunction interruptInits[] = 
    {
        &initKeyboardInterrupt
    };

    for(uint8_t i = 0; i < ARRAY_SIZE(interruptInits); i++)
    {
        interruptInits[i]();
    }
}