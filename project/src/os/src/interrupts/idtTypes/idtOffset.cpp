#include "idtOffset.h"

void setOffset(InterruptDescriptor* desc, uint64_t offset)
{
    desc->offsetLow  = OFFSET_LOW_BIT_MASK( offset);
    desc->offsetMid  = OFFSET_MID_BIT_MASK( offset);
    desc->offsetHigh = OFFSET_HIGH_BIT_MASK(offset);
}

uint64_t getOffset(InterruptDescriptor* desc)
{
    uint64_t offset = 0;

    // using bitwise or in order to set the value offset with the right bits:
    offset |= ((uint64_t)desc->offsetLow )       ;
    offset |= ((uint64_t)desc->offsetMid )  << 16;
    offset |= ((uint64_t)desc->offsetHigh)  << 32;

    return offset;
}