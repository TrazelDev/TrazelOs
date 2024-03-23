#pragma once
#include "scheduler.hpp"

class RoundRobin : public Scheduler
{
public:
    virtual bool addProcess() override;
private:

};