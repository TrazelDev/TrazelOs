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
        value /= DECIMAL_DIGIT_COUNT;
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
/// once to translate a variable and then called again and still use the value of the first call
template <typename IntegerType>
static char* intToString(IntegerType value)
{
    static char intToStringBuffer[INTEGER_MAX_LENGTH];

    if(value == 0)
    {
        intToStringBuffer[0] = '0' ;
        intToStringBuffer[1] = NULL;
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

    intToStringBuffer[integerLength] = NULL;
    while (value != 0)
    {
        integerLength--;
        
        // putting the current digit in the most right side to create the number:
        digit = ((value % DECIMAL_DIGIT_COUNT) * (isNegative ? NEGATIVE : POSITIVE)) + '0';
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


/// @brief this functions is getting an hex number and returns the length of the hex number + the length of the prefix 0x
/// @tparam HexType the type of the hex always unsigned
/// @param value the value that we would like to know the length of
/// @return the length of the value + the length of the prefix 0x
template <typename HexType>
static uint8_t getHexLength(HexType value)
{
    uint8_t count = HEX_PREFIX_LENGTH;

    while (value != 0)
    {
        value /= HEX_DIGIT_COUNT;
        count++;
    }

    return count;
}


uint8_t getHexLength(uint8_t  value) { return getHexLength<uint8_t >(value); };
uint8_t getHexLength(uint16_t value) { return getHexLength<uint16_t>(value); };
uint8_t getHexLength(uint32_t value) { return getHexLength<uint32_t>(value); };
uint8_t getHexLength(uint64_t value) { return getHexLength<uint64_t>(value); };


/// @brief function that translates hexes to char* (string)
/// @tparam HexType the type of the hex that needs to be translated
/// @param value the value that we would like to translate to string
/// @return a pointer to a char this pointer is to a static variable that is defined in the function that means that this function cannot be called
/// once to translate a variable and then called again and still use the value of the first call
template <typename HexType>
static char* hexToString(HexType value)
{
    static char hexToStringBuffer[HEX_MAX_LENGTH];

    // setting the start of the number which is always the following prefix
    hexToStringBuffer[0] = '0';
    hexToStringBuffer[1] = 'x';
    
    // in the case that the value is 0:
    if(!value)
    {
        hexToStringBuffer[2] = '0';
        hexToStringBuffer[3] = NULL;
        return hexToStringBuffer;
    }

    uint8_t hexLength = getHexLength(value);
    uint8_t digit     = 0;

    hexToStringBuffer[hexLength] = NULL;
    while (value != 0)
    {
        hexLength--;
        
        // getting the value value that is in this index: 
        digit = (value % HEX_DIGIT_COUNT);
        
        // if the value is above the decimal then using the letters:
        if(digit > DECIMAL_DIGIT_COUNT - 1) { digit += 'A' - DECIMAL_DIGIT_COUNT; }
        else                                { digit += '0'                      ; }

        hexToStringBuffer[hexLength] = digit;

        value /= HEX_DIGIT_COUNT;
    }

    return hexToStringBuffer;
}


char* hexToString(uint8_t  value) { return hexToString<uint8_t >(value); };
char* hexToString(uint16_t value) { return hexToString<uint16_t>(value); };
char* hexToString(uint32_t value) { return hexToString<uint32_t>(value); };
char* hexToString(uint64_t value) { return hexToString<uint64_t>(value); };