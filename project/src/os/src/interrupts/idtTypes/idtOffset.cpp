#include "idtOffset.h"

void setOffset(InterruptDescriptor* desc, uint64_t offset)
{
    desc->offsetLow  = SET_OFFSET_LOW_BIT_MASK( offset);
    desc->offsetMid  = SET_OFFSET_MID_BIT_MASK( offset);
    desc->offsetHigh = SET_OFFSET_HIGH_BIT_MASK(offset);
}

uint64_t getOffset(InterruptDescriptor* desc)
{
    uint64_t offset = 0;

    // using bitwise or in order to set the value offset with the right bits:
    offset |= GET_OFFSET_LOW_BIT_MASK(desc );
    offset |= GET_OFFSET_MID_BIT_MASK(desc );
    offset |= GET_OFFSET_HIGH_BIT_MASK(desc);

    return offset;
}