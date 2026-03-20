#pragma once

#define GET_PROCESS_SIZE(processAddr) ((uint64_t)processAddr)
#define GET_PROCESS_STARTING_ADDRESS(processAddr) ((uint64_t)(&processAddr + 1))
#define GET_NEXT_PROCESS(processAddr) (uint64_t*)(((uint64_t)(&processAddr)) + (uint64_t)processAddr)