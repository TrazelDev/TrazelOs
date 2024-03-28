#include "tests/test.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"


void my_function() __attribute__((section(".specific_address")));
void my_function() {
    printf("hello world\n");
}
void testingFunction()
{
    // processesInSystem[1].func();
    
    uint64_t oldIp;
    //asm volatile ("lea (%%rip), %0" : "=r" (oldIp));
    // asm volatile ("pushq %0" : : "r" (oldIp + 16) : "memory");

    //asm volatile("call  %0\n\t" : "=a"(newIp));
    printf("test");
}


extern char _end[];
typedef void (*Func)();
extern "C" void _start()
{
    initPrint();
    initPageFrameAllocator();
    initIDT();
    initMemoryPool();

    IF_DEBUG(testEverything());
    printf("Address: %x\n", &_end);
    printf("Address: %x\n", &my_function);
    Func func = (Func)_end;
    func();
}