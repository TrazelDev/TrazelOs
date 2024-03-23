#include "sysCalls.h"
#include "utility/print/print.h"

void sysExit(uint64_t code)
{
    printf("Process has been terminated with error code: %d", code);
    asm volatile("hlt"); // for now just halting the cpu
}