#pragma once
#include "utility/utility.h"
#include "idtTypes/interruptDescriptor.h"
#include "idtTypes/idtOffset.h"
#include "pic/pic.h"

/// @brief initializing the idt
void initIDT();

// this is the definition of idt register which is 10 bytes instead of the usual 8 bytes registers that in 64 bit system
// the first 2 bytes ( dw ) are the size of the idt, and the second 8 bytes ( dq ) are the first address of the idt
typedef struct 
{
    uint16_t size;
    uint64_t idtAddress;
} __attribute__((packed)) IdtRegister;

/// @brief the function loads the interrupt register to memory which loads the interrupt descriptor table to memory.
/// this function all also sets the interrupt flag which enables interrupts to get called in the os.
static void loadIDTRegister();

/// @brief this function is used to initialize a specific interrupt function
/// @param isrAddress the address of the function is called when an interrupt is triggered
/// @param interruptStackTable a special part of the stack for interrupts instead of the regular stack of the os
/// @param typesAndAttributes the type of the interrupt software hardware and so on
/// @param extraBits extra bits that could be used as flags currently it is nothing
/// @param interruptIndex the index of the interrupt
void initSpecificInterrupt(uint64_t isrAddress, uint8_t interruptStackTable, uint8_t typesAndAttributes, uint8_t extraBits, uint8_t interruptIndex);


