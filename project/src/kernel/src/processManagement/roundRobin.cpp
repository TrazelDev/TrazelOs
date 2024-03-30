#include "roundRobin.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "src/memory/pageRequestor.h"

static ProcessList* m_processList;
static ProcessList* m_processListEnd;
static size_t MaxQuantumTime;
void initScheduler(size_t quantumTime)
{
    m_processListEnd = nullptr;
    m_processList = nullptr;
    quantumTime = quantumTime;
}

Process* createProcess(size_t processesSize, uint64_t processesStartingAddress)
{
    Process* newProcess = (Process*)malloc_x(sizeof(Process));
    newProcess->pcb     = (ProcessControlBlock*)malloc_x(sizeof(ProcessControlBlock));

    newProcess->pcb->processId    = generateUniqueProcessId();
    newProcess->pcb->processState = PROCESS_STATE::READY;
    newProcess->pcb->cpuRegisters = CpuRegisters(); // creating cpu register which contain the value 0
    
    VirtualAddress stackStartingAddress = generateStackPages(PROCESS_STACK_PAGE_COUNT);
    // newProcess->pcb->pageTablePtr       = ge
    newProcess->pcb->stackPointer       = getStackLastAddress(stackStartingAddress).raw;
    newProcess->pcb->instructionPointer = processesStartingAddress;
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

void removeProcess(Process* process)
{
    ASSERT_PRINT_ERROR(m_processList, printf("Error: there are no process"));
    ProcessList* currentProcess = m_processList;
    m_processList = m_processList->next;

    free_x(currentProcess->process, sizeof(Process));
    free_x(currentProcess, sizeof(ProcessList));
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

uint64_t generateUniqueProcessId()
{
    static uint64_t processId = 0;
    processId++;

    return processId;
}
VirtualAddress generateStackPages(size_t pagesCount)
{
    VirtualAddress stackFirstAddress = requestPages(pagesCount);
}
VirtualAddress getStackLastAddress(VirtualAddress stackStartingAddress)
{
    VirtualAddress stackLastAddress;
    stackLastAddress.raw = stackStartingAddress.raw;
    stackLastAddress.raw += ((PAGE_SIZE * PROCESS_STACK_PAGE_COUNT) - sizeof(size_t));

    return stackLastAddress;
}