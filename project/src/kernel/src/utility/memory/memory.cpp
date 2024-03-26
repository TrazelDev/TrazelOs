#include "memory.h"

template <typename BufferChar>
static void memset(BufferChar* pointer, BufferChar value, uint64_t size)
{
    while(size-- > 0)
    {
        *(pointer) = value;
        pointer++;
    }
}


void memset(uint8_t*  pointer, uint8_t  value, uint64_t size) { return memset<uint8_t >(pointer, value, size); };
void memset(uint16_t* pointer, uint16_t value, uint64_t size) { return memset<uint16_t>(pointer, value, size); };
void memset(uint32_t* pointer, uint32_t value, uint64_t size) { return memset<uint32_t>(pointer, value, size); };
void memset(uint64_t* pointer, uint64_t value, uint64_t size) { return memset<uint64_t>(pointer, value, size); };

void memset(int8_t*  pointer, int8_t  value, uint64_t size) { return memset<int8_t >(pointer, value, size);};
void memset(int16_t* pointer, int16_t value, uint64_t size) { return memset<int16_t>(pointer, value, size);};
void memset(int32_t* pointer, int32_t value, uint64_t size) { return memset<int32_t>(pointer, value, size);};
void memset(int64_t* pointer, int64_t value, uint64_t size) { return memset<int64_t>(pointer, value, size);};

void memset(void* pointer, int64_t value, uint64_t size) { return memset<int64_t>((int64_t*)pointer, value, size);};

void pushAllRegisters()
{
    asm volatile 
    (
        "pushq %rax\n\t"
        "pushq %rbx\n\t"
        "pushq %rcx\n\t"
        "pushq %rdx\n\t"
        "pushq %rsi\n\t"
        "pushq %rdi\n\t"
        "pushq %rbp\n\t"
        "pushq %rsp\n\t"
        "pushq %r8\n\t"
        "pushq %r9\n\t"
        "pushq %r10\n\t"
        "pushq %r11\n\t"
        "pushq %r12\n\t"
        "pushq %r13\n\t"
        "pushq %r14\n\t"
        "pushq %r15\n\t"
    );
}
void popAllRegisters()
{
    asm volatile 
    (
        "popq %r15\n\t"
        "popq %r14\n\t"
        "popq %r13\n\t"
        "popq %r12\n\t"
        "popq %r11\n\t"
        "popq %r10\n\t"
        "popq %r9\n\t"
        "popq %r8\n\t"
        "popq %rsp\n\t"
        "popq %rbp\n\t"
        "popq %rdi\n\t"
        "popq %rsi\n\t"
        "popq %rdx\n\t"
        "popq %rcx\n\t"
        "popq %rbx\n\t"
        "popq %rax\n\t"
    );
}
