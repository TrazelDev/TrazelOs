#include "memory.h"

template <typename BufferChar>
static void memset(BufferChar* pointer, BufferChar value, uint64_t size)
{
    while(size-- > 0)
    {
        *(pointer) = value;
        pointer++;
    }
}


void memset(uint8_t*  pointer, uint8_t  value, uint64_t size) { return memset<uint8_t >(pointer, value, size); };
void memset(uint16_t* pointer, uint16_t value, uint64_t size) { return memset<uint16_t>(pointer, value, size); };
void memset(uint32_t* pointer, uint32_t value, uint64_t size) { return memset<uint32_t>(pointer, value, size); };
void memset(uint64_t* pointer, uint64_t value, uint64_t size) { return memset<uint64_t>(pointer, value, size); };

void memset(int8_t*  pointer, int8_t  value, uint64_t size) { return memset<int8_t >(pointer, value, size);};
void memset(int16_t* pointer, int16_t value, uint64_t size) { return memset<int16_t>(pointer, value, size);};
void memset(int32_t* pointer, int32_t value, uint64_t size) { return memset<int32_t>(pointer, value, size);};
void memset(int64_t* pointer, int64_t value, uint64_t size) { return memset<int64_t>(pointer, value, size);};