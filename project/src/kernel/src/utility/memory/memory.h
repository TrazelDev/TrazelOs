#pragma once
#include "utility/types/types.h"


void memset(uint8_t*  pointer, uint8_t  value, uint64_t size);
void memset(uint16_t* pointer, uint16_t value, uint64_t size);
void memset(uint32_t* pointer, uint32_t value, uint64_t size);
void memset(uint64_t* pointer, uint64_t value, uint64_t size);

void memset(int8_t*  pointer, int8_t  value, uint64_t size);
void memset(int16_t* pointer, int16_t value, uint64_t size);
void memset(int32_t* pointer, int32_t value, uint64_t size);
void memset(int64_t* pointer, int64_t value, uint64_t size);
void memset(void* pointer, int64_t value, uint64_t size);

/// @brief function to push all of the register so when something is switched then 
// we can come back with the same registers to continue the commands normally.
void pushAllRegisters();
void popAllRegisters();