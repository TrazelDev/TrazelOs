#pragma once
#include <types.h>

enum SPECIAL_CHARS
{
	NEW_LINE = '\n',
	BACK_SPACE = '\b'
};

void set_cursor_position(uint16_t position);


/*
 * Takes x,y cordinates and translates to the correct vga text mode position
 * x - max of VGA_WIDTH
 * y - max of VGA_HEIGHT
 * returns - the correct position the the function set_cursor_position takes
 */
uint16_t position_to_cordinates(uint8_t x, uint8_t y);


void print_decimal_uint64(uint64_t value);
void print_decimal_int64(int64_t value);

void print_hex_uint64(uint64_t value);
void print_decimal_int64(int64_t value);

void print_string(const char* str);
void print_char(char ch);
