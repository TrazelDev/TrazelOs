#pragma once
#include "processControlBlock.h"

struct Process
{
    ProcessControlBlock* process;
};

class Scheduler
{
public:
    virtual bool addProcess() = 0; 
};