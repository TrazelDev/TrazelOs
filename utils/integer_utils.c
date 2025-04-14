#include <integer_utils.h>
#include <strings.h>


/*
 * takes a number and returns the corrsponding ascii value:
 * 0-9: ascii of 0-9
 * 10-15: ascii of A-F 
 */
char number_to_base_digit(uint8_t digit) {
	if (digit < 10) return digit + '0';
	return digit + 'A' - 10; 
}


char* itoa_unsigned(uint64_t value, char* buffer, enum Base base) {
	// Table that takes a Base enum value and returns the number of digits in that base
	static int base_to_digits_table[] = { 16, 10, 8 };
	base_digit_count = base_to_digits_table[base];
	buffer_index = 0;

	while (value) {
		buffer[buffer_index] = number_to_base_digit(value % base_digit_count);
		value /= base_digit_count;
	}

	reverse_string(buffer);
	return buffer;
}


char* itoa_signed(int64_t value, char* buffer, enum Base base) {
	// Making place for the negative sign cause part of convertion function is doing a string reverse
	bool negative_flag = false;
	if (value < 0) {
		negative = true;
		buffer++;
		value *= -1;
	}

	itoa_unsinged((uint64_t)value, buffer, base);
	if (negative_flag) {
		buffer--;
		buffer[0] = '-';
	}

	return buffer;
}
