#pragma once
#include "processControlBlock.h"

#define PROCESS_STACK_PAGE_COUNT 3
struct Process
{
    ProcessControlBlock* pcb;
};

struct ProcessList
{
    Process* process;
    ProcessList* next;
};

void initScheduler(size_t quantumTime);
Process* createProcess(size_t processesSize, uint64_t processesStartingAddress);
void addProcess(Process* process);
void removeProcess(Process* process);
Process* frontProcess(Process* process);
bool containsProcess();

uint64_t generateUniqueProcessId();
VirtualAddress generateStackPages(size_t pagesCount);
VirtualAddress getStackLastAddress(VirtualAddress stackStartingAddress);
uint64_t generateProcessPageTable(VirtualAddress stackStartingVAddress, size_t stackPageCount, size_t processStartingAddress, size_t processPageCount, ProcessControlBlock* processPCB);

void contextSwitch(Process* process) __attribute__((section(".contextSwitch_location")));