#pragma once
#include "../kernel/src/utility/types/types.h"

#define MY_EXEC_HEADER { \
  0x9000,  \
  0x1000,  \
  0x1000,  \
}

struct MyExecutableHeader 
{
  uint32_t entryPoint; // Address of the process entry point (e.g., _start)
  uint32_t codeSize;   // Size of the code segment in bytes
  uint32_t dataSize;   // Size of the data segment in bytes
};

// Place this structure at the beginning of your process source code
struct MyExecutableHeader header = MY_EXEC_HEADER;