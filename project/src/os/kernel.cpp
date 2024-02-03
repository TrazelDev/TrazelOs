#include "src/utility/print.h"

extern "C" void _start()
{
    setCursorPosition(postionFromCords(0, 0));
    printString("hello world");
}