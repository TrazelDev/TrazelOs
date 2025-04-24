#pragma once
#include <types.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint8_t*)0xB8000
#define VGA_LAST_MEMORY_ADDRESS (0xB8000 + (VGA_WIDTH * VGA_HEIGHT * 2))

enum SPECIAL_CHARS
{
    NEW_LINE = '\n',
    BACK_SPACE = '\b'
};

#define POSITION_LOWER_BYTES(position) (uint8_t)( position        & 0xFF)
#define POSITION_UPPER_BYTES(position) (uint8_t)((position >> 8 ) & 0xFF)


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
