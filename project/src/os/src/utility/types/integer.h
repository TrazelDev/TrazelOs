#pragma once
#include "utility/types/types.h"

// the lengths of several things:
#define INTEGER_MAX_LENGTH  21 // the max length that is needed to represent an integer including the possible minus sign and null
#define HEX_MAX_LENGTH      19 // the max length that is needed to represent a hex number including the 0x length and null
#define HEX_PREFIX_LENGTH   2  // the lengths of the '0x' that is the start of a string

// the amount of digits in different bases
#define DECIMAL_DIGIT_COUNT 10
#define HEX_DIGIT_COUNT     16

#define NEGATIVE            -1
#define POSITIVE            1

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



/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(uint8_t  value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(uint16_t value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(uint32_t value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(uint64_t value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(char      value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(short     value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(int       value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* intToString(long long value);



/// @brief this functions is getting an hex number and returns the length of the hex number + the length of the prefix 0x
uint8_t getHexLength(uint8_t  value);
/// @brief this functions is getting an hex number and returns the length of the hex number + the length of the prefix 0x
uint8_t getHexLength(uint16_t value);
/// @brief this functions is getting an hex number and returns the length of the hex number + the length of the prefix 0x
uint8_t getHexLength(uint32_t value);
/// @brief this functions is getting an hex number and returns the length of the hex number + the length of the prefix 0x
uint8_t getHexLength(uint64_t value);


/// @brief function that translates hexes to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* hexToString(uint8_t  value);
/// @brief function that translates hexes to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* hexToString(uint16_t value);
/// @brief function that translates hexes to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* hexToString(uint32_t value);
/// @brief function that translates hexes to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
char* hexToString(uint64_t value);