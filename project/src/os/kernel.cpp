#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "tests/test.h"

extern "C" void _start()
{
    initPrint();
    initIDT();
    
    IF_DEBUG(testEverything());    
    printf("current tests are successful\n");
}