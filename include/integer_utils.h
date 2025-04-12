#pragma once
#include <types.h>


/*
 * The bases that are supported
 */
enum Base {
	HEX = 1,
	DECIMAL,
	OCTAL,
};


/*
 * Int to string unsinged, takes an int and converts to a string in a desired base.
 * value - the int that needs to be converted into a string
 * buffer - the buffer that the strings needs to be loaded to
 * base - the base it needs to be converted into
 * Return value - if the buffer is big enough then it returns the buffer address else it returns a NULL
 */
char* itoa_unsigned(uint64_t value, char* buffer, enum Base base);
char* itoa_signed(int64_t value, char* buffer, enum Base base);
