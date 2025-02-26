#pragma once

#ifdef DEBUG

// when the condition that is inside of the assert does not evaluate to true then the action that is passed is being performed and then
// a hlt instruction is being performed on the cpu which freezes it until an interrupt is being triggered
#define ASSERT_PRINT_ERROR(condition, action)          \
    if(!condition)                          \
    {                                        \
        action;                               \
        asm volatile("hlt");                   \
    }

#define ASSERT(condition)                  \
    if(!condition)                          \
    {                                        \
        asm volatile("hlt");                  \
    }
#else

#define ASSERT_PRINT_ERROR(condition, action)
#define ASSERT(condition) 

#endif // end of DEBUG