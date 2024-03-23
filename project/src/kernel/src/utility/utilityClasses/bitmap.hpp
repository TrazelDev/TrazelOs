#pragma once 
#include "utility/types/types.h"



#define INDEX_OUT_OF_BOUNDS_ERROR "Error: index out of bounds\n"

#define BITS_IN_BYTE 8
#define GET_BIT_MASK_OF_SPECIFIC_BIT(index) (0b10000000 >> index)
#define GET_BIT_INSIDE_OF_BYTE_MASK(byte, indexInByte) (byte & GET_BIT_MASK_OF_SPECIFIC_BIT(indexInByte))
class BitMap
{
public:
    /// @brief the constructor that creates a bit map
    /// @param buffer the buffer that the bit map can contain since the os doesn't support malloc when this bit map is used
    /// and also this bit map will also be used for the physical page manager which will span across multiple pages
    /// @param size the size of the buffer in bytes
    BitMap(uint8_t* buffer, uint64_t sizeInBytes) : m_buffer(buffer), m_sizeInBits(sizeInBytes) { }
    BitMap() : m_buffer(NULL), m_sizeInBits(NULL) { }


    /// @brief function to reset the values of the bitmap:    
    inline void resetBitmapValues(uint8_t* buffer, uint64_t sizeInBits) 
    {
        m_sizeInBits = sizeInBits;
        m_buffer     = buffer;
    }


    inline uint8_t*  getBuffer()   { return m_buffer;     }
    inline uint64_t  getBitCount() { return m_sizeInBits; }


    bool operator[](uint64_t index);
    void setBit(uint64_t index, bool value);
private:
    uint64_t m_sizeInBits;   // the amount of bits that are in the buffer
    uint8_t* m_buffer;
};