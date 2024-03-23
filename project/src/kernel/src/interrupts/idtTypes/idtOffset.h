#pragma once
#include "utility/utility.h"
#include "interruptDescriptor.h"


// ways to configure the offset, the offset is address of the isr function that is being called when the interrupt is triggered:
#define SET_OFFSET_LOW_BIT_MASK( offset)      (uint16_t)((offset & 0x000000000000FFFF)      )
#define SET_OFFSET_MID_BIT_MASK( offset)      (uint16_t)((offset & 0x00000000FFFF0000) >> 16)
#define SET_OFFSET_HIGH_BIT_MASK(offset)      (uint32_t)((offset & 0xFFFFFFFF00000000) >> 32)


/// @brief this function is getting the offset which is the address of the isr function that we want to be called when the interrupt is triggered
void    setOffset(InterruptDescriptor* desc, uint64_t offset);
#define setIsrFunctionAddress(desc, offset) setOffset(desc, offset)


// ways to extract the offset:
#define GET_OFFSET_LOW_BIT_MASK( desc) ((uint64_t)desc->offsetLow )
#define GET_OFFSET_MID_BIT_MASK( desc) ((uint64_t)desc->offsetMid )  << 16
#define GET_OFFSET_HIGH_BIT_MASK(desc) ((uint64_t)desc->offsetHigh)  << 32

/// @brief function that will return the offset from the descriptor:
/// @return the offset 
uint64_t getOffset(InterruptDescriptor* desc);



// interrupt types:

// there are two main types of interrupts interrupts which are interrupt gates which are hardware interrupts when certain hardware input is 
// given then the interrupt is being called.
#define IDT_TA_InterruptGate 0b10001110  

// the second type of interrupt is trap gate interrupt which is a software interrupt, this interrupt is being performed 
// when a certain operation in the software is being performed
#define IDT_TA_TrapGate      0b10001111

// there is a third type of gate known as a Call Gate, which is part of the Task State Segment(TSS) 
// and is used in task switching and calling functions across privilege levels.
// call Gate is used in the context of task switching and inter-privilege level function calls,
// it allows programs to call functions in different privilege levels.
#define IDT_TA_CallGate      0b10001100