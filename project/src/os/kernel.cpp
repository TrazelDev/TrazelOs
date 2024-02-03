#include "utility/utility.h"
#include "tests/test.h"

extern "C" void _start()
{
    setCursorPosition(postionFromCords(0, 0));
    

    IF_DEBUG(testEverything());
    printString("success\n");
}