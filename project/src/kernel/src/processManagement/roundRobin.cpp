#include "roundRobin.h"
#include "src/memory/dynamicMemory/freeMemoryPool.h"
#include "src/memory/virtualMemory/virtualMemoryManager.h"
#include "src/memory/pageRequestor.h"

static ProcessList* m_processList;
static ProcessList* m_processListEnd;
static size_t MaxQuantumTime;
extern uint64_t* _contextSwitch_addr;
__attribute__((section(".globalPCB_location"))) static ProcessControlBlock globalPCB;

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
    newProcess->pcb->pageTablePtr       = generateProcessPageTable(stackStartingAddress, PROCESS_STACK_PAGE_COUNT, processesStartingAddress, processesSize / PAGE_SIZE);
    newProcess->pcb->stackPointer       = getStackLastAddress(stackStartingAddress).raw;
    newProcess->pcb->instructionPointer = processesStartingAddress;

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

uint64_t generateProcessPageTable(VirtualAddress stackStartingVAddress, size_t stackPageCount, size_t processStartingAddress, size_t processPageCount)
{
    PhysicalAddress processPlm4Ptr    = createPhysicalAddr(requestPages(1).raw);
    PhysicalAddress processFirstPAddr = createPhysicalAddr(processStartingAddress);
    VirtualAddress  processFirstVAddr = createVirtualAddr((uint64_t)0);

    // mapping ths process first code pages:
    for(size_t i = 0; i < processPageCount; i++)
    {
        if(!mapMemory(processFirstPAddr, processFirstVAddr, false, processPlm4Ptr))
        {
            printf("hi");
        }
        processFirstPAddr.raw += PAGE_SIZE;
        processFirstVAddr.raw += PAGE_SIZE;
    }

    // mapping ths process first stack pages:
    PhysicalAddress stackStartingPAddress = getPhysicalAddress(stackStartingVAddress);
    for(size_t i = 0; i < stackPageCount + 1; i++)
    {
        if(!mapMemory(stackStartingPAddress, processFirstVAddr, false, processPlm4Ptr))
        {
            printf("hi");
        }
        stackStartingPAddress.raw += PAGE_SIZE;
        processFirstVAddr.raw += PAGE_SIZE;
    }
    
    
    // mapping the context switch function:
    mapMemory(createPhysicalAddr((uint64_t)&_contextSwitch_addr), createVirtualAddr((uint64_t)&_contextSwitch_addr), true, processPlm4Ptr);
    mapMemory(createPhysicalAddr((uint64_t)0xB8000), createVirtualAddr((uint64_t)0xB8000), true, processPlm4Ptr);

    //printf("\n\n%x\n", createPhysicalAddr(processStartingAddress));
    //printf("%x\n", getPhysicalAddress(createVirtualAddr((uint64_t)0x10), processPlm4Ptr));
    printf("te: %x\n", processFirstVAddr.raw - 8);
    
    return processPlm4Ptr.raw;
}

__attribute__((section(".main_location"))) int main()
{
    //*((unsigned char*)0xB8000) = 'a';
    return 5;
}

typedef int (*ProcessFunc)();
void contextSwitch(Process* process)
{
    static int value = 0;
    static uint64_t* ptr1 = 0;
    printf("%x\n", &globalPCB);
    

    globalPCB = process->pcb;
    // printf("\n\n\n%x\n", getPhysicalAddress(createVirtualAddr((uint64_t)8), createPhysicalAddr(globalPCB.pageTablePtr)));

    // switching all the registers rcx - r15:
    asm volatile (
        "movq  %0 , %%rbx  \n\t"
        "movq  %1 , %%rcx  \n\t"
        "movq  %2 , %%rdx  \n\t"
        "movq  %3 , %%rsi  \n\t"
        "movq  %4 , %%rdi  \n\t"
        "movq  %5 , %%rbp  \n\t"
        "movq  %6 , %%r8   \n\t"
        "movq  %7 , %%r9   \n\t"
        "movq  %8 , %%r10  \n\t"
        "movq  %9 , %%r11  \n\t"
        "movq  %10, %%r12  \n\t"
        "movq  %11, %%r13  \n\t"
        "movq  %12, %%r14  \n\t"
        "movq  %13, %%r15  \n\t"
        : 
        : "r"(globalPCB.cpuRegisters.flags), "r"(globalPCB.cpuRegisters.rcx), "r"(globalPCB.cpuRegisters.rdx),
        "r"(globalPCB.cpuRegisters.rsi), "r"(globalPCB.cpuRegisters.rdi), "r"(globalPCB.cpuRegisters.rbp), "r"(globalPCB.cpuRegisters.r8),
        "r"(globalPCB.cpuRegisters.r9), "r"(globalPCB.cpuRegisters.r10), "r"(globalPCB.cpuRegisters.r11), "r"(globalPCB.cpuRegisters.r12),
        "r"(globalPCB.cpuRegisters.r13), "r"(globalPCB.cpuRegisters.r14), "r"(globalPCB.cpuRegisters.r15)
        : "memory"
    );


    // switching rbx and flags:
    asm volatile (
        "pushq  %%rbx     \n\t"
        "popfq            \n\t"
        "movq   %0, %%rbx \n\t"
        :
        : "r"(globalPCB.cpuRegisters.rbx)
        :
    );
       
    
    asm volatile
    (
        "mov %0   , %%rax \n\t"   
        "mov %%rax, %%cr3 \n\t"
        :
        : "r"(globalPCB.pageTablePtr)
        :
    );
    
    // switching stack pointer:
    asm volatile (
        "movq $0x6FF8, %%rsp \n\t"
        :
        : // "r"(globalPCB.stackPointer)
        :
    );

    //asm volatile("push $5");
    if(!value)
    {
        ptr1 = (uint64_t*)(0x0);
        //*ptr1 = 5;
        value = (uint64_t)(0x0);
    }
    if(value)
    {
    }
    ((ProcessFunc)((void*)(0x8)))();
    //main();
    asm volatile("cli");
    asm volatile("hlt");
    
    

    /*
    asm volatile (
        "pushq %0    \n\t"
        "popq  %%rax \n\t"
        :
        : "r"(globalPCB.cpuRegisters.rax)
        :
    );
    */


    /*
    asm volatile
    (
        "mov %0   , %%rax \n\t"   
        "mov %%rax, %%cr3 \n\t"
        :
        : "r"(value)
        : "%rax"
    );
    */
}