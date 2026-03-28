#include "bitmap.h"

#define BITS_IN_BYTE 8
struct tz_bitmap bitmap_create(void* buffer, uint64_t bit_count) {
	struct tz_bitmap bitmap = (struct tz_bitmap){.bitmap = buffer, .bit_count = bit_count};

	for (uint64_t i = 0; i < (bit_count + BITS_IN_BYTE - 1) / BITS_IN_BYTE; i++) {
		bitmap.bitmap[i] = 0;
	}

	return bitmap;
}

int bitmap_set_bit_value(struct tz_bitmap* bitmap, uint64_t bit_index, bool value) {
	if (bit_index >= bitmap->bit_count) {
		return -1;
	}

	uint64_t byte_index = bit_index / BITS_IN_BYTE;
	uint64_t bit_offset = bit_index % BITS_IN_BYTE;
	uint8_t mask = 1 << bit_offset;

	if (value) {
		bitmap->bitmap[byte_index] |= mask;
	} else {
		bitmap->bitmap[byte_index] &= ~mask;
	}

	return 0;
}

int bitmap_get_bit_value(struct tz_bitmap* bitmap, uint64_t bit_index) {
	if (bit_index >= bitmap->bit_count) {
		return -1;
	}

	uint64_t byte_index = bit_index / BITS_IN_BYTE;
	uint64_t bit_offset = bit_index % BITS_IN_BYTE;

	return (bitmap->bitmap[byte_index] >> bit_offset) & 1;
}
