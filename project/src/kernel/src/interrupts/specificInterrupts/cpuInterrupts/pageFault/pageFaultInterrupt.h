// this an auto generated file
#pragma once

#include "utility/utility.h"
/**
 * Bit 0 (Present): 0 if the page is not present, 1 if the page is present.
 * Bit 1 (Write): 0 for a read access, 1 for a write access.
 * Bit 2 (User/Supervisor): 0 for kernel-mode access, 1 for user-mode access.
 * Bit 3 (Reserved): This bit is reserved and should always be 0.
 * Bit 4 (Instruction/Non-instruction): 0 for a data access, 1 for an instruction fetch.
 * Bit 5 (SGX): Software Guard Extensions (SGX) related, should be 0 for normal page faults.
 * Bits 6-11 (Reserved): These bits are reserved and should always be 0.
 * Bits 12-51 (Physical Address): The physical address of the page that caused the fault.
 * Bits 52-62 (Reserved): These bits are reserved and should always be 0.
 * Bit 63 (NX - No Execute): 0 if the page is executable, 1 if it's non-executable.
*/
union
{
    struct
    {
        uint64_t present          : 1;
        uint64_t writable         : 1;
        uint64_t kernelOrUserMode : 1;
        uint64_t reserved1        : 1;
        uint64_t isInstruction    : 1;
        uint64_t sgx              : 1;
        uint64_t reserved2        : 6;
        uint64_t physicalAddress  : 40;
        uint64_t reserved3        : 11;
        uint64_t isExecutable     : 1;  
    } controlBits;
    
    uint64_t raw;
} typedef PageFaultErrorCode;


/// @brief initializes the interrupt itself
void initPageFaultInterrupt();


/// @brief the isr function that is being triggered when and interrupt occurs
extern "C" void isrPageFaultHandler(PageFaultErrorCode error);