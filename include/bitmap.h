#pragma once
#include <include/types.h>

struct tz_bitmap {
	size_t bit_count;
	uint8_t* bitmap;
};

struct tz_bitmap bitmap_create(void* buffer, uint64_t bit_count);

/** Gets a tz_bitmap and set a bit at bit_index to a specific value.
 * @param bitmap a pointer to the tz_bitmap struct.
 * @param bit_index the index of the bit to set.
 * @param value the value to set the bit to (true for 1, false for 0).
 * @return 0 if success, -1 if bit_index is out of bounds.
 */
int bitmap_set_bit_value(struct tz_bitmap* bitmap, uint64_t bit_index, bool value);

/** Gets a tz_bitmap and returns the value of the bit at bit_index.
 * @param bitmap a pointer to the tz_bitmap struct.
 * @param bit_index the index of the bit to get.
 * @return the value of the bit at bit_index (1 or 0), or -1 if bit_index is out of bounds.
 */
int bitmap_get_bit_value(struct tz_bitmap* bitmap, uint64_t bit_index);
