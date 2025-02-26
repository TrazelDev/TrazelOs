#include "roundRobin.h"
#include "contextSwitchAsmMacros.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "src/memory/virtualMemory/virtualMemoryManager.h"
#include "src/memory/pageRequestor.h"

extern uint64_t* _contextSwitch_addr;
static ProcessList* m_processList;
static ProcessList* m_processListEnd;
static size_t MaxQuantumTime;

static uint64_t stackPtr;
__attribute__((section(".plm4TablePtr_location"))) static uint64_t plm4TablePtr;
__attribute__((section(".globalPCB_location"   ))) static ProcessControlBlock globalPCB;
__attribute__((section(".kernelPCB_location"   ))) static ProcessControlBlock kernelPCP;

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
    newProcess->pcb->pageTablePtr       = generateProcessPageTable(stackStartingAddress, PROCESS_STACK_PAGE_COUNT, processesStartingAddress, processesSize / PAGE_SIZE, newProcess->pcb);

    return newProcess;
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
    return stackFirstAddress;
}
VirtualAddress getStackLastAddress(VirtualAddress stackStartingAddress)
{
    VirtualAddress stackLastAddress;
    stackLastAddress.raw = stackStartingAddress.raw;
    stackLastAddress.raw += ((PAGE_SIZE * PROCESS_STACK_PAGE_COUNT) - sizeof(size_t));

    return stackLastAddress;
}


uint64_t generateProcessPageTable(VirtualAddress stackStartingVAddress, size_t stackPageCount, size_t processStartingAddress, 
    size_t processPageCount, ProcessControlBlock* processPCB)
{
    PhysicalAddress processPlm4Ptr        = createPhysicalAddr(requestPages(1).raw);
    VirtualAddress  processFirstVAddr     = createVirtualAddr((uint64_t)&_contextSwitch_addr);
    PhysicalAddress processFirstPAddr     = createPhysicalAddr(processStartingAddress);
    PhysicalAddress stackStartingPAddress = getPhysicalAddress(stackStartingVAddress);

    mapMemory(createPhysicalAddr((uint64_t)&_contextSwitch_addr), processFirstVAddr, true, processPlm4Ptr);
    processFirstVAddr.raw += PAGE_SIZE;


    processPCB->instructionPointer = processFirstVAddr.raw + sizeof(uint64_t);
    for(size_t i = 0; i < processPageCount; i++)
    {
        ASSERT_PRINT_ERROR(mapMemory(processFirstPAddr, processFirstVAddr, false, processPlm4Ptr), printf("Error: failed to map memory of process\n"));
        processFirstPAddr.raw += PAGE_SIZE;
        processFirstVAddr.raw += PAGE_SIZE;
    }
    for(size_t i = 0; i < stackPageCount + 1; i++)
    {
        ASSERT_PRINT_ERROR(mapMemory(stackStartingPAddress, processFirstVAddr, false, processPlm4Ptr), printf("Error: failed to map memory of process\n"));
        stackStartingPAddress.raw += PAGE_SIZE;
        processFirstVAddr.raw     += PAGE_SIZE;
    }
    processPCB->stackPointer = processFirstVAddr.raw - sizeof(uint64_t);

    mapMemory(createPhysicalAddr((uint64_t)0xB8000), createVirtualAddr((uint64_t)0xB8000), true, processPlm4Ptr);
    return processPlm4Ptr.raw;
}

typedef int (*ProcessFunc)();
void contextSwitch(Process* process)
{
    printf("instruction ptr: %x \n", process->pcb->instructionPointer);
    PUSH_ALL_REGISTERS();
    SAVE_STACK_PTR(stackPtr);
    SAVE_PLM4_TABLE(plm4TablePtr);

    globalPCB = process->pcb;
    RESTORE_PROCESS_REGISTERS(globalPCB.cpuRegisters);
    RESTORE_PROCESS_FLAGS(globalPCB.cpuRegisters);
    RESTORE_PROCESS_PML4_TABLE(globalPCB.pageTablePtr);
    RESTORE_PROCESS_STACK_PTR(globalPCB.stackPointer);
    
    TRIGGER_PROCESS(ProcessFunc, globalPCB.instructionPointer);

    RESTORE_PLM4_TABLE(plm4TablePtr);
    RESTORE_STACK_PTR(stackPtr);
    POP_ALL_REGISTER();
}