#include "integer.h"

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
uint8_t getIntLength(uint8_t   value) { return getIntLength<uint8_t  >(value); }
uint8_t getIntLength(uint16_t  value) { return getIntLength<uint16_t >(value); }
uint8_t getIntLength(uint32_t  value) { return getIntLength<uint32_t >(value); }
uint8_t getIntLength(uint64_t  value) { return getIntLength<uint64_t >(value); }
uint8_t getIntLength(char      value) { return getIntLength<char     >(value); }
uint8_t getIntLength(short     value) { return getIntLength<short    >(value); }
uint8_t getIntLength(int       value) { return getIntLength<int      >(value); }
uint8_t getIntLength(long long value) { return getIntLength<long long>(value); }


/// @brief function that translates ints to char* (string)
/// @tparam IntegerType the type of int that needs to be translated to string
/// @param value the value that needs translation
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first
template <typename IntegerType>
static char* intToString(IntegerType value)
{
    static char intToStringBuffer[INTEGER_MAX_LENGTH];

    if(value == 0)
    {
        intToStringBuffer[0] = '0' ;
        intToStringBuffer[1] = '\0';
        return intToStringBuffer;
    }

    uint8_t integerLength = getIntLength(value);
    bool isNegative = false;
    char digit;
    if(value < 0)
    {
        intToStringBuffer[0] = '-';
        isNegative = true;
    }

    intToStringBuffer[integerLength] = '\0';
    while (value != 0)
    {
        integerLength--;
        
        // putting the current digit in the most right side to create the number:
        digit = ( (value % DECIMAL_DIGIT_COUNT) * (isNegative ? NEGATIVE : POSITIVE) ) + '0';
        intToStringBuffer[integerLength] = digit;

        value /= DECIMAL_DIGIT_COUNT;
    }

    return intToStringBuffer;
}


char* intToString(uint8_t   value) { return intToString<uint8_t  >(value); }
char* intToString(uint16_t  value) { return intToString<uint16_t >(value); }
char* intToString(uint32_t  value) { return intToString<uint32_t >(value); }
char* intToString(uint64_t  value) { return intToString<uint64_t >(value); }
char* intToString(char      value) { return intToString<char     >(value); }
char* intToString(short     value) { return intToString<short    >(value); }
char* intToString(int       value) { return intToString<int      >(value); }
char* intToString(long long value) { return intToString<long long>(value); }
