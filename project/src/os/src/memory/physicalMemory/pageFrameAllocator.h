#include "utility/utility.h"
#include "utility/utilityClasses/bitmap.hpp"
#include "physicalAddress.h"

/// @brief creating page frame allocator of a bit map. this functions is called from outside of this file to initialize everything.
void initPageFrameAllocator();

/// @brief setting the a bitMap class instance with the buffer of the bit map in the inputted address this bit map is used in
/// a way that each bit represents one page in the system and it's says if the page is taken or not
/// @param bitmapStartingAddress the address where the buffer of the bitmap is being put
/// @param bitMapSize the amount of bits that are in the bitmap ( )
static void initPageBitMap(PhysicalAddress bitmapStartingAddress, uint64_t bitMapSize);