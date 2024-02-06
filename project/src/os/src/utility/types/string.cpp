#include "string.h"

bool strCmp(const char* str1, const char* str2)
{
    while(*str1 != NULL && *str2 != NULL)
    {
        // if the values in the strings are different then returning that the strings are not equal
        if((*str1) != (*str2))
        {
            return false;
        }
        
        str1++;
        str2++;
    }

    // in the case that one of the strings is shorter then the first one and at first the strings are the same:
    return (*str1 == *str2);
}

uint64_t strLength(const char* str)
{
    uint64_t count = 0;
    while(*str != NULL)
    {
        count++;
        str++;
    }

    return count;
}