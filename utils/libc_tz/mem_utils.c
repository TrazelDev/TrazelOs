#include <mem_utils.h>

void* memcpy(const void* dest, const void* src, uint64_t n) {
	uint8_t* curr_dest = (uint8_t*)dest;
	uint8_t* curr_src = (uint8_t*)src;

	for (uint64_t i = 0; i < n; i++) {
		curr_dest[i] = curr_src[i];
		curr_dest++;
		curr_src++;
	}

	return (void*)dest;
}
