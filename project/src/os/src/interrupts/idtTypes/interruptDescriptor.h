#pragma once
#include "utility/utility.h"
#include "idtSelector.h"
#include "idtIndexes.h"

typedef struct
{  
  uint16_t offsetLow;
  IdtSelector selector; // the code segment that the isr (interrupt service routine ) function is located at
  
  uint8_t  ist;           // the interrupt stack table it is 0 for ( interrupt gate and trap gate which are explained above ) 
  #define NO_INTERRUPT_STACK_TABLE 0

  uint8_t  typesAttr;    // the type of interrupt explained below
  uint16_t offsetMid;
  uint32_t offsetHigh;
  uint32_t extraBits; // extra bits that are used for alignment ( in the future may they can be used for flags ) 
  #define NO_INTERRUPT_EXTRA_BITS 0
} __attribute__((packed)) InterruptDescriptor;