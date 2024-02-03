#pragma once
#include "utility/types/types.h"

#define INTEGER_MAX_LENGTH 21
#define NEGATIVE           -1
#define DECIMAL_DIGIT_COUNT 10
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
/// once to translate a variable and then called again and still use the value of the first
char* intToString(uint8_t  value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(uint16_t value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(uint32_t value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(uint64_t value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(char      value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(short     value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(int       value);
/// @brief function that translates ints to char* (string)
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
char* intToString(long long value);
