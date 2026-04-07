#pragma once
#include <include/types.h>

void init_msr_cpu();

uint64_t msr_get_register(uint32_t msr);
void msr_set_register(uint64_t msr, uint64_t value);
