#pragma once
#include "utility/utility.h"
#include "src/memory/virtualMemory/virtualMemoryTypes.h"

struct CpuRegisters
{
    uint64_t rax;   // General purpose register (accumulator)
    uint64_t rbx;   // General purpose register (base)
    uint64_t rcx;   // General purpose register (counter)
    uint64_t rdx;   // General purpose register (data)
    uint64_t rsi;   // General purpose register (source index)
    uint64_t rdi;   // General purpose register (destination index)
    uint64_t rsp;   // Stack pointer
    uint64_t rbp;   // Base pointer
    uint64_t rip;   // Instruction pointer
    uint64_t r8;    // General purpose register
    uint64_t r9;    // General purpose register
    uint64_t r10;   // General purpose register
    uint64_t r11;   // General purpose register
    uint64_t r12;   // General purpose register
    uint64_t r13;   // General purpose register
    uint64_t r14;   // General purpose register
    uint64_t r15;   // General purpose register
    uint64_t flags; // Status register
    uint64_t cs;    // Code segment register
    uint64_t ds;    // Data segment register
    uint64_t es;    // Extra segment register
    uint64_t fs;    // More extra segment register
    uint64_t gs;    // Still more extra segment register
    uint64_t ss;    // Stack segment register
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
    ProcessControlBlock() = default;

    uint64_t       processId;
    uint64_t       programCounter;
    uint64_t       stackPointer;
    PagePML4Table* pageTablePtr;
    CpuRegisters   cpuRegisters;
    PROCESS_STATE  processState;
};