#include "roundRobin.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"


static ProcessList* m_processList;
static ProcessList* m_processListEnd;
void initScheduler()
{
    m_processListEnd = nullptr;
    m_processList = nullptr;
}

void addProcess(Process* process)
{
    if(m_processList)
    {
        m_processListEnd->next = (ProcessList*)malloc_x(sizeof(ProcessList));
        m_processListEnd = m_processListEnd->next;
        m_processListEnd->process = process;
        return;
    }

    m_processList = (ProcessList*)malloc_x(sizeof(ProcessList));
    m_processListEnd = m_processList;
    m_processListEnd->process = process;
}

Process* removeProcess(Process* process)
{
    ASSERT_PRINT_ERROR(m_processList, printf("Error: there are no process"));
    ProcessList* currentProcess = m_processList;
    Process* requestProcess = m_processList->process;

    m_processList = m_processList->next;
    free_x(currentProcess, sizeof(ProcessList));

    return requestProcess;
}

Process* frontProcess(Process* process) 
{
    ASSERT_PRINT_ERROR(m_processList, printf("Error: there are no process"));
    return m_processList->process;
}

bool containsProcess()
{
    return m_processList;
}