#pragma once

#define PUSH_ALL_REGISTERS() \
    asm volatile (           \
        "push %rax \n\t"     \
        "push %rbx \n\t"     \
        "push %rcx \n\t"     \
        "push %rdx \n\t"     \
        "push %rsi \n\t"     \
        "push %rdi \n\t"     \
        "push %rbp \n\t"     \
        "push %r8  \n\t"     \
        "push %r9  \n\t"     \
        "push %r10 \n\t"     \
        "push %r11 \n\t"     \
        "push %r12 \n\t"     \
        "push %r13 \n\t"     \
        "push %r14 \n\t"     \
        "push %r15 \n\t"     \
    );

#define POP_ALL_REGISTER() \
    asm volatile (         \
        "pop %r15 \n\t"    \
        "pop %r14 \n\t"    \
        "pop %r13 \n\t"    \
        "pop %r12 \n\t"    \
        "pop %r11 \n\t"    \
        "pop %r10 \n\t"    \
        "pop %r9  \n\t"    \
        "pop %r8  \n\t"    \
        "pop %rbp \n\t"    \
        "pop %rdi \n\t"    \
        "pop %rsi \n\t"    \
        "pop %rdx \n\t"    \
        "pop %rcx \n\t"    \
        "pop %rbx \n\t"    \
        "pop %rax \n\t"    \
        "sti      \n\t"    \
    );

#define SAVE_STACK_PTR(stackPtr)      \
    asm volatile (                    \
        "movq %%rsp, %0\n\t"          \
        : "=r"(stackPtr)              \
        :                             \
        :                             \
    );                        

#define RESTORE_STACK_PTR(stackPtr)   \
    asm volatile(                     \
        "movq %0, %%rbx \n\t"         \
        "movq %%rax, %%rsp \n\t"      \
        :                             \
        : "r"(stackPtr)               \
        :                             \
    );  

#define SAVE_PLM4_TABLE(plm4TablePtr) \
    asm volatile (                   \
        "movq %%cr3, %%rax \n\t"     \
        "movq %%rax, %0    \n\t"     \  
        : "=r"(plm4TablePtr)         \ 
        :                            \
        :                            \
    );

#define RESTORE_PLM4_TABLE(plm4TablePtr) \
    asm volatile (                       \
        "movq %0   , %%rax \n\t"         \
        "movq %%rax, %%cr3 \n\t"         \
        :                                \
        : "r"(plm4TablePtr)              \
        :                                \
    );

#define RESTORE_PROCESS_REGISTERS(cpuRegisters) \
    asm volatile (               \
        "movq  %0 , %%rbx  \n\t" \
        "movq  %1 , %%rcx  \n\t" \
        "movq  %2 , %%rdx  \n\t" \
        "movq  %3 , %%rsi  \n\t" \
        "movq  %4 , %%rdi  \n\t" \
        "movq  %5 , %%rbp  \n\t" \
        "movq  %6 , %%r8   \n\t" \
        "movq  %7 , %%r9   \n\t" \
        "movq  %8 , %%r10  \n\t" \
        "movq  %9 , %%r11  \n\t" \
        "movq  %10, %%r12  \n\t" \
        "movq  %11, %%r13  \n\t" \
        "movq  %12, %%r14  \n\t" \
        "movq  %13, %%r15  \n\t" \
        :                        \
        :                        \
        "r"(cpuRegisters.flags), "r"(cpuRegisters.rcx), "r"(cpuRegisters.rdx),                     \
        "r"(cpuRegisters.rsi), "r"(cpuRegisters.rdi), "r"(cpuRegisters.rbp), "r"(cpuRegisters.r8), \
        "r"(cpuRegisters.r9), "r"(cpuRegisters.r10), "r"(cpuRegisters.r11), "r"(cpuRegisters.r12), \
        "r"(cpuRegisters.r13), "r"(cpuRegisters.r14), "r"(cpuRegisters.r15)                        \
        : "memory" \
    );

#define RESTORE_PROCESS_FLAGS(cpuRegisters) \
    asm volatile (                          \
        "pushq  %%rbx     \n\t"             \
        "popfq            \n\t"             \
        "movq   %0, %%rbx \n\t"             \
        :                                   \
        : "r"(cpuRegisters.rbx)             \
        :                                   \
    );

#define RESTORE_PROCESS_PML4_TABLE(pageTablePtr) \
    asm volatile (                       \ 
        "mov %0   , %%rax \n\t"          \    
        "mov %%rax, %%cr3 \n\t"          \ 
        :                                \          
        : "r"(pageTablePtr)              \  
        :                                \  
    );

#define RESTORE_PROCESS_STACK_PTR(stackPtr) \
    asm volatile (                          \
        "movq %0, %%rsp \n\t"               \
        :                                   \
        : "r"(stackPtr)                     \
        :                                   \
    );

#define TRIGGER_PROCESS(processFuncType, processStartingAddr) ((processFuncType)((void*)(processStartingAddr)))()
#define RESTORE