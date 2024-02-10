#pragma once
#include "utility/types/types.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint8_t*)0xB8000
#define VGA_LAST_MEMORY_ADDRESS (0xB8000 + (VGA_WIDTH * VGA_HEIGHT * 2))

enum class SPECIAL_CHARS
{
    NEW_LINE = '\n',
    BACK_SPACE = '\b'
};

#define POSITION_LOWER_BYTES(position) (uint8_t)( position        & 0xFF)
#define POSITION_UPPER_BYTES(position) (uint8_t)((position >> 8 ) & 0xFF)
/// @brief function that is changing the position of the cursor on the screen:
/// @param position the new position of the cursor where things will be printed
void setCursorPosition(uint16_t position);

/// @brief function that translates an x and y coordinates into a cursor position:
/// @param x the x coordinate
/// @param y the y coordinate
/// @return the cursor position of this two coordinates
uint16_t postionFromCords(uint8_t x, uint8_t y);


void printInt(uint8_t   value);
void printInt(uint16_t  value);
void printInt(uint32_t  value);
void printInt(uint64_t  value);
void printInt(char      value);
void printInt(short     value);
void printInt(int       value);
void printInt(long long value);


/// @brief function that is printing an string
/// @param str the string that needs to be printed and it is terminated with a null
void printString(const char* str);

void printChar(char ch);


void printHex(uint8_t  value);
void printHex(uint16_t value);
void printHex(uint32_t value);
void printHex(uint64_t value);