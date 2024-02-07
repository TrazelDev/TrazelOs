#pragma once
#include "utility/utility.h"

#define GDT_INDICATOR                      0
#define SAME_PRIVILEGE_AS_DESCRIPTOR_TABLE 0
#define CODE_SEGMENT                       1

/// @brief the selector values are telling us where the isr function ( interrupt service routine ) is located at what privileges it has
/// in what kind of descriptor table it is located at and so on
typedef union
{
    struct
    {
        // Requestor Privilege Level (RPL): 
        // 00 - the same as the descriptor
        // 01 - next higher privilege level 
        // 10 - reserved 11
        uint16_t privilegeLevel : 2 ;      
        uint16_t tableIndicator : 1 ;     // table indicator ( false: GDT - our case, true: LDT )
        uint16_t index          : 13;     // index into the GDT or LDT, 0 - null, 1 - code segment, 2 - data segment and so on
    } attr;
    uint16_t raw;


    /// @brief the function that is setting the selector in the current way we want it to be configured:
    /// gdt, the same privilege level as the descriptor table and in the code segment
    void setSelector();
} __attribute__((packed)) IdtSelector;