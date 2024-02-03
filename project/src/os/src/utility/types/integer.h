#pragma once
#include "utility/types/types.h"

#define INTEGER_MAX_LENGTH 21

/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(uint8_t  value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(uint16_t value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(uint32_t value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(uint64_t value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(char      value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(short     value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(int       value);
/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
uint8_t getIntLength(long long value);




char* intToString(uint8_t  value);
char* intToString(uint16_t value);
char* intToString(uint32_t value);
char* intToString(uint64_t value);
char* intToString(char      value);
char* intToString(short     value);
char* intToString(int       value);
char* intToString(long long value);
