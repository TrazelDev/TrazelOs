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