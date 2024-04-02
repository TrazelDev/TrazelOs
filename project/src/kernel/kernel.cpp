#include "tests/test.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"
#include "src/memory/pageRequestor.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "src/processManagement/roundRobin.h"



extern uint64_t* _next_process;
extern uint64_t* _contextSwitch_addr;
typedef int (*ProcessFunc)();
extern "C" void _start()
{
    initPrint();
    printf("\n");
    initPageFrameAllocator();
    initIDT();
    initMemoryPool();

    IF_DEBUG(testEverything());
    
    contextSwitch(createProcess((uint64_t)_next_process, (uint64_t)(&_next_process + 1)));
    printf("%x\n", (&_next_process + 1));

    //printf("herllo");
    //ProcessFunc processesPrint = (ProcessFunc)(void*)(&_next_process + 1);
    //processesPrint();
    //printf("he");
    //
    //uint64_t val = (uint64_t)_next_process;
    //printf("%x\n", val);
    //printf("%x\n", (&_next_process + 1));

    //printf("%x\n", _next_process);*/
//
    //uint64_t val = (uint64_t)(_next_process) + (uint64_t)(&_next_process);
    //uint64_t* ptr = (uint64_t*)val;
    //printf("%x\n", ptr);
    
    //processesPrint = (ProcessFunc)(void*)(ptr + 1);
    //processesPrint();
//
    //val = (uint64_t)(ptr) + ((uint64_t)(val) / 8);
    //ptr = (uint64_t*)val;
    //
    //processesPrint = (ProcessFunc)(void*)(ptr + 1);
    //processesPrint();
    //printf("%x\n", ptr);

    //printf("%x\n", *((uint64_t*)(0x16000)));

    //printf("%x,\n", _next_process + 1);
    //printf("%x,\n", _next_process + 2);
    //printf("%x,\n", _next_process + 3);

    //asm volatile("call  %0\n\t" : "=a"(addr));
    //printf("%x,\n", _next_process + 2);

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
