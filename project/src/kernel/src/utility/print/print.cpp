#include "print.h"
#include "printVga.h"
#include "utility/types/types.h"
#include "utility/debug/debug.h"

void initPrint()
{
    setCursorPosition(postionFromCords(0, 0));
    DEBUG_LOG("Initialized printf\n");
}


// Since infinite params are creating a weird stack frame then we need to add the following when we want to get the second param from the first on.
// Also there can only be max of around 20 params and the function is compiled with optimization level of 0 to make 
// the stack frame constant in all configurations
#define AMOUNT_TO_GET_FIRST_PARAM 4

void __attribute__((optimize("-O0"))) printf(const char* format, ...)
{
    void* param = ((&format) + AMOUNT_TO_GET_FIRST_PARAM);

    while(*format != NULL)
    {
        if(*format == SPECIAL_SYMBOL_PREFIX && isSpecialSymbol(*(format + 1)))
        {
            switch(*(format + 1))
            {
            case (int)SPECIAL_SYMBOLS::CHAR:
                printChar(*((char*)param));
                break;
            case (int)SPECIAL_SYMBOLS::INTEGER:
                printInt(*((int32_t*)param));
                break;
            case (int)SPECIAL_SYMBOLS::STRING:
                printString(*((char**)param));
                break;
            case (int)SPECIAL_SYMBOLS::UNSIGNED_INTEGER:
                printInt(*((uint64_t*)param));
                break;
            case (int)SPECIAL_SYMBOLS::HEX:
                printHex(*((uint64_t*)param));
                break;
            default:
                break;
            }

            param = ((uint8_t*)param) + sizeof(uint64_t); // adding the size of address to go to the next address
            format += 2;                                  // since this special chars are 2 chars % and something else 
            continue;
        }

        printChar(*format);
        

        // going to the next char:
        format++;
    }
}

static bool isSpecialSymbol(char ch)
{
    switch(ch)
    {
    case (int)SPECIAL_SYMBOLS::CHAR:
        return true;
    case (int)SPECIAL_SYMBOLS::INTEGER:
        return true;
    case (int)SPECIAL_SYMBOLS::STRING:
        return true;
    case (int)SPECIAL_SYMBOLS::UNSIGNED_INTEGER:
        return true;
    case (int)SPECIAL_SYMBOLS::HEX:
        return true;
    default:
        return false;
    }
}