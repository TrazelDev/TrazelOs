#include "utility/utility.h"
#include "tests/test.h"

extern "C" void _start()
{
    initPrint();
    
    IF_DEBUG(testEverything());    
    printf("success\n");


}