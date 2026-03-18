#pragma once
#include <types.h>

/*
 * The bases that are supported
 */
enum integer_base {
	INTEGER_BASE_HEX = 0,
	INTEGER_BASE_DECIMAL = 1,
	INTEGER_BASE_OCTAL = 2,
};

/*
 * Int to string unsigned, takes an int and converts to a string in a desired base.
 * value - the int that needs to be converted into a string
 * buffer - the buffer that the strings needs to be loaded to
 * base - the base it needs to be converted into
 * Return value - the inputted buffer address
 */
char* itoa_unsigned(uint64_t value, char* buffer, enum integer_base base);
char* itoa_signed(int64_t value, char* buffer, enum integer_base base);
