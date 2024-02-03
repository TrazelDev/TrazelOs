#include "integer.h"

/*
/// @brief function that translates ints to char* (string)
/// @tparam IntegerType the type of int that needs to be translated to string
/// @param value the value that needs translation
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
template <typename IntegerType>
static char* intToString(IntegerType value)
{
    static char* intToStringBuffer[INTEGER_MAX_LENGTH];

    if(value == 0)
    {
        intToStringBuffer[0] = '0' ;
        intToStringBuffer[0] = '\0';
    }


}
*/

/// @brief function that gets the number and returns the number of digits in the number plus 1 if the number is negative
/// @tparam IntegerType the type of int that we need to know its length
/// @return the length of the number
template <typename IntegerType>
static uint8_t getIntLength(IntegerType value)
{
    uint8_t count = 0;

    // adding the length of the minus sign:
    if(value < 0) { count++; }

    while (value != 0)
    {
        value /= 10;
        count++;
    }

    return count;
}
uint8_t getIntLength(uint8_t  value) { return getIntLength<uint8_t >(value); }
uint8_t getIntLength(uint16_t value) { return getIntLength<uint16_t>(value); }
uint8_t getIntLength(uint32_t value) { return getIntLength<uint32_t>(value); }
uint8_t getIntLength(uint64_t value) { return getIntLength<uint64_t>(value); }
uint8_t getIntLength(char      value) { return getIntLength<char     >(value); }
uint8_t getIntLength(short     value) { return getIntLength<short    >(value); }
uint8_t getIntLength(int       value) { return getIntLength<int      >(value); }
uint8_t getIntLength(long long value) { return getIntLength<long long>(value); }