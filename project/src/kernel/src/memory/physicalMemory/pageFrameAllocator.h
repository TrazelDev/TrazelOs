#pragma once
#include "utility/utility.h"
#include "utility/utilityClasses/bitmap.hpp"
#include "pageFrameAllocatorDefs.h"


/// @brief creating page frame allocator of a bit map. this functions is called from outside of this file to initialize everything.
void initPageFrameAllocator();

/// @brief function that returns a physical address that is not taken and not identity mapped by the kernel
PhysicalAddress requestUserPage();
/// @brief this function returns a physical address that is identity mapped by the boot loader
PhysicalAddress requestIdentityMappedPage();
void returnPhysicalPage(PhysicalAddress);

/// @brief setting the a bitMap class instance with the buffer of the bit map in the inputted address this bit map is used in
/// a way that each bit represents one page in the system and it's says if the page is taken or not
/// @param bitmapStartingAddress the address where the buffer of the bitmap is being put
/// @param bitMapSize the amount of bits that are in the bitmap
static void initPageBitMap(PhysicalAddress bitmapStartingAddress, uint64_t bitMapSize);

static void reserveReservedMemoryPages();
static void lookKernelPages();
static void lookSpecialDataPages();

/// @brief this function is changing the status of a specific page by changing it's value in the bit map
/// and also updating the usedMemory, reservedMemory and freeMemory according to the options the user select
/// @param addr the physical address of the page
/// @param releaseOrGrabPage this value is telling whether to to turn the bit of that says that the page is taken on or off, 
/// true - release, false - grab
/// @param lockOrUnreserve if we lock the page or free the page we specify put the address freeMemory because we would like the status of the freeMemory
/// to be updated and if we want to reserve the memory or unreserve we specify the address of reservedMemory
static void changePageStatus(PhysicalAddress addr, bool releaseOrGrabPage, uint64_t* lockOrUnreserve);
/**
 * @brief this function is changing the status of a specific pages by changing there value in the bit map
 * and also updating the usedMemory, reservedMemory and freeMemory according to the options the user select
 * @param addr the physical address of the page
 * @param releaseOrGrabPage this value is telling whether to to turn the bit of that says that the page is taken on or off, 
 * true - release, false - grab
 * @param lockOrUnreserve if we lock the page or free the page we specify put the address freeMemory because we would like the status of the freeMemory
 * to be updated and if we want to reserve the memory or unreserve we specify the address of reservedMemory
 * @param pageCount the amount of pages in a raw that we would like to change their status
*/
static void changePagesStatus(PhysicalAddress addr, bool releaseOrGrabPage, uint64_t* lockOrUnreserve, uint64_t pageCount);


static PhysicalAddress getPageWithinRange(uint64_t startingRange, uint64_t endRange);

#ifdef DEBUG

void printInformation();
void printTakenPages();

#endif 