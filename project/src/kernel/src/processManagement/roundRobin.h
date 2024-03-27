#pragma once
#include "processControlBlock.h"

struct Process
{
    ProcessControlBlock* process;
};

struct ProcessList
{
    Process* process;
    ProcessList* next;
};


void initScheduler();
void addProcess(Process* process);
Process* removeProcess(Process* process);
Process* frontProcess(Process* process);
bool containsProcess();
