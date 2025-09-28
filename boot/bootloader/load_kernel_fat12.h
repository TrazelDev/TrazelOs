#pragma once
#include <types.h>

// TODO: 1. I think researching now about the actuall way to load the elf is best
// cause then you would understand exactly what your filing system needs to do
// 2. Making the memory map is also very important now so I can know exactly what regions
// can I use now
void init_readonly_fat12();
uint64_t* get_kernel(char* kernel_name, uint64_t* kernel_load_point);

uint64_t* load_kernel(char* kernel_filename, uint64_t* location);
