#pragma once
#include "utility/utility.h"
#include "src/memory/virtualMemory/virtualMemoryTypes.h"

struct CpuRegisters
{
    uint64_t rax;   
    uint64_t rbx;   
    uint64_t rcx;   
    uint64_t rdx;   
    uint64_t rsi;   
    uint64_t rdi;   
    uint64_t rbp;     
    uint64_t r8;    
    uint64_t r9;    
    uint64_t r10;   
    uint64_t r11;   
    uint64_t r12;   
    uint64_t r13;   
    uint64_t r14;   
    uint64_t r15;   
    uint64_t flags;

    // generating all zero cpu registers:
    CpuRegisters()
    {
        rax    = 0;
        rbx    = 0;
        rcx    = 0;
        rdx    = 0;
        rsi    = 0;
        rdi    = 0;
        rbp    = 0;
        r8     = 0; 
        r9     = 0; 
        r10    = 0;
        r11    = 0;
        r12    = 0;
        r13    = 0;
        r14    = 0;
        r15    = 0;
        flags  = 0;
    }
};

inline __attribute__((always_inline)) void replaceCpuRegisters(CpuRegisters& regs)
{
    asm volatile (
        "movq %0, %%rax\n\t"
        "movq %1, %%rbx\n\t"
        "movq %2, %%rcx\n\t"
        "movq %3, %%rdx\n\t"
        "movq %4, %%rsi\n\t"
        "movq %5, %%rdi\n\t"
        "movq %6, %%rbp\n\t"
        "movq %7, %%r8\n\t"
        "movq %8, %%r9\n\t"
        "movq %9, %%r10\n\t"
        "movq %10, %%r11\n\t"
        "movq %11, %%r12\n\t"
        "movq %12, %%r13\n\t"
        "movq %13, %%r14\n\t"
        "movq %14, %%r15\n\t"
        "pushfq\n\t"
        "popq %15\n\t"
        : 
        : "m"(regs.rax), "m"(regs.rbx), "m"(regs.rcx), "m"(regs.rdx),
        "m"(regs.rsi), "m"(regs.rdi), "m"(regs.rbp), "m"(regs.r8),
        "m"(regs.r9), "m"(regs.r10), "m"(regs.r11), "m"(regs.r12),
        "m"(regs.r13), "m"(regs.r14), "m"(regs.r15), "m"(regs.flags)
        : "memory"
    );


}

enum class PROCESS_STATE
{
    RUNNING    = 1,
    WAITING    = 2,
    READY      = 3,
    TERMINATED = 4
};


struct ProcessControlBlock
{
    uint64_t       processId;
    CpuRegisters   cpuRegisters;
    uint64_t       stackPointer;
    uint64_t       instructionPointer;
    uint64_t       pageTablePtr;
    PROCESS_STATE  processState;

    ProcessControlBlock()
    {
        processId          = 0;
        cpuRegisters       = CpuRegisters();
        stackPointer       = 0;
        instructionPointer = 0;
        pageTablePtr       = 0;
        processState       = PROCESS_STATE::READY;
    }
    ProcessControlBlock(ProcessControlBlock* other)
    {
        processId          = other->processId;
        cpuRegisters       = other->cpuRegisters;
        stackPointer       = other->stackPointer;
        instructionPointer = other->instructionPointer;
        pageTablePtr       = other->pageTablePtr;
        processState       = other->processState;
    }
};