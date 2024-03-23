#pragma once

void initPrint();


#define SPECIAL_SYMBOL_PREFIX '%'
enum class SPECIAL_SYMBOLS
{
    CHAR             = 'c',
    INTEGER          = 'd',
    STRING           = 's',
    UNSIGNED_INTEGER = 'u',
    HEX              = 'x',
};

void printf(const char* format, ...);

static bool isSpecialSymbol(char ch);