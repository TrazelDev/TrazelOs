#pragma once
#include "processControlBlock.h"

struct ProcessList
{
    Process* process;
    ProcessList* next;
};

struct Process
{
    ProcessControlBlock* process;
};

void initScheduler();
void addProcess(Process* process);
Process* removeProcess(Process* process);
Process* frontProcess(Process* process);
bool containsProcess();
