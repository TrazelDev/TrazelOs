#include "io.h"

/**
 * this functions is using the inline asm it allows to write asm code from inside of a c code
 * in this specific example the %0, %1 are the operand that go into the standard instruction of outb in asm 
 * "a"(val): The "a" constraint tells the compiler to store the operand val in the eax register
 * "Nd"(port): The "Nd" constraint is a little more complex. 
 * The N means that the value must be an 8-bit constant. 
 * The d means that the compiler should use the edx register. 
 * However, in the case of the outb instruction, the port number can be specified directly in the instruction itself 
 * (this is known as an “immediate” operand). 
 * So, the edx register is not actually used here, and the port value is embedded directly into the instruction.
*/
void outb(unsigned short port, unsigned char val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
    // in asm it will look something like this:
    /*
        mov eax, val
        mov dx, port
        out dx, al
    */
}

unsigned char inb(unsigned short port)
{
    unsigned char returnValue;
    asm volatile("inb %1, %0" : "=a"(returnValue): "Nd"(port));
    // will look something like this:
    /*
        mov dx, port
        in al, dx
        mov val, eax
    */
    
    return returnValue;
}