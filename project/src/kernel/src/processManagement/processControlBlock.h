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
    uint64_t rsp;   
    uint64_t rbp;   
    uint64_t rip;   
    uint64_t r8;    
    uint64_t r9;    
    uint64_t r10;   
    uint64_t r11;   
    uint64_t r12;   
    uint64_t r13;   
    uint64_t r14;   
    uint64_t r15;   
    uint64_t flags;
};

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
    PagePML4Table* pageTablePtr;
    PROCESS_STATE  processState;
};