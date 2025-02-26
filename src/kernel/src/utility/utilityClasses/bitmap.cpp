#include "bitmap.hpp"
#include "utility/utility.h"

bool BitMap::operator[](uint64_t index)
{
    ASSERT_PRINT_ERROR(index >= m_sizeInBits, printf(INDEX_OUT_OF_BOUNDS_ERROR)) 
    uint8_t bitIndexInByte = index % BITS_IN_BYTE;
    
    // getting the specific bit that we wanted:
    return (GET_BIT_INSIDE_OF_BYTE_MASK(m_buffer[index / BITS_IN_BYTE], bitIndexInByte));
}

void BitMap::setBit(uint64_t index, bool value)
{
    ASSERT_PRINT_ERROR(index >= m_sizeInBits, printf(INDEX_OUT_OF_BOUNDS_ERROR)) 
    uint8_t bitIndexInByte = index % BITS_IN_BYTE;
    

    
    uint8_t specificBitMask = GET_BIT_MASK_OF_SPECIFIC_BIT((index % BITS_IN_BYTE)); // the bit that we need need turned on
    if (value)
    {
        m_buffer[index / BITS_IN_BYTE] |= specificBitMask;
        return;
    }

    m_buffer[index / BITS_IN_BYTE] &= ~specificBitMask;
}