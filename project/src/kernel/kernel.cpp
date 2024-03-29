#include "tests/test.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"


void my_function() __attribute__((section(".specific_address")));
void my_function(){
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


extern uint64_t _next_process[];
typedef int (*Func)();
extern "C" void _start()
{
    initPrint();
    initPageFrameAllocator();
    initIDT();
    initMemoryPool();

    IF_DEBUG(testEverything());
    printf("%x,\n", _next_process);
    printf("%x,\n", _next_process[0]);

    void* addr = _next_process;
    Func func_ptr = (Func)addr;
    func_ptr();
    printf("hello world");
    // printf("%x,\n", _next_process + 2);

    //printf("%x,\n", *(_end + 1));
    //printf("%x,\n", *((uint64_t*)0x74a0));
    //int flag = 0;
    //for(uint64_t i = 0;;i--)
    //{
    //    if(_end[i])
    //    {
    //        printf("%d, %x, %x\n", i, _end[i], _end + i);
    //        if(flag == 5) { break; }
    //        flag++;
    //    }
    //}

    //for(int i = 0; i < 1000000000; i++)
    //{
        //if(_end[i] == 1) {printf("%x\n", i); }
        //{
        //    printf("%x, %x\n", i, (uint64_t*)(_end));
//
        //    break;
        //}
    //}
    // printf("Address: %x\n", &my_function);

}

/*
0x10160: 0x30 , 0x78, 0x30, 0x00, 0x30, 0x30, 0x30, 0x00
0xf2a8: 0x01, 7 * 0x0
0x11398

*/