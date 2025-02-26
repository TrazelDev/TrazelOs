#pragma once
#include "utility/utility.h"
#include "virtualMemoryTypes.h"

// this is used when we want to switch pte ( page table entry to a new page ) this is only doing it for the current process
// if multiprocessing is being used then it will not work for the other processors ( this invalidates the automatic tlb )
#define FLUSH_BUFFER(addr) asm volatile("invlpg (%0)" ::"r" (addr) : "memory")

/// @brief returning the value in cr3 register that is a pointer to the plm4 table 
PhysicalAddress getPML4Address();

/// @brief the function is getting a virtual address and converts it back to a physical one
PhysicalAddress getPhysicalAddress(VirtualAddress vAddr, PhysicalAddress plm4Address = getPML4Address());

/// @brief the function is getting a physical address and a virtual address and makes it so the entire page that the virtual address 
/// points to, will be mapped to the entire page that the physical address points to.
/// @param pAddr some physical address inside of a specific page we want to map
/// @param vAddr some virtual address inside of a specific page we want to map
/// @param overrunTableEntry defines what is happening in the case that the last page table entree ( in the actual pte ) is already mapped to another page
/// @return whether the mapping succeeded or not
bool mapMemory(PhysicalAddress pAddr, VirtualAddress vAddr, bool overrunTableEntry = false, PhysicalAddress plm4Address = getPML4Address());


/// @brief function to unmap the memory from the physical memory by making the page not physical and pointing to null
/// @param vAddr the virtual address that we want to unmap and make free
void unmapMemory(VirtualAddress vAddr);

/// @brief the function is getting a page index and returning the corrsponding page table that in this page index 
static inline PageTable* getPageTable(uint64_t pageIndex);

/// @brief this functions gets the page a page index that contains some variant of a page table and an index in this page table 
/// which are the components to get a page table entree and then it returns the index of the page of the next page table page or the requested page
/// @param pageIndex this index represents the index of the page that the page table is in if the entire memory is considered 
/// as buffer where each entree is at the size PAGE_SIZE
/// @param entryIndex this is an entree index inside of the page table that the value page index points to
/// @return the page index of the next page table or the page that we are looking for it's memory
static inline uint64_t getNextPageIndex(uint64_t pageIndex, uint64_t entryIndex);

/// @brief function that is getting Page table an index to this page and puts in there a new page which is being requested from the physical memory manager
/// or puts in there the index that is in physicalPageIndex, in the case that the flag isPageTableIndex is false 
/// @param currentPageTable the page table
/// @param entryIndex the index in the page table
/// @param isPageTableIndex represents if the page table index is memory that the user wants to use and not the page for the purpose of page table
/// @param physicalPageIndex the index that we put in the page table in the case that the flag isPageTableIndex is false
static inline void setNewPageEntry(PageTable* currentPageTable, uint64_t entryIndex, bool isPageTableIndex, uint64_t physicalPageIndex);

/// @brief this function getting page index that in it there is page table, and an index in that table ( entryIndex ). If the page table in that index
/// has the flag present turned off in the entryIndex spot ( which means that the page is not present in the system ) we request a new identity mapped page from 
/// the physical memory manager and we put the page index of the page we got in the original table in the spot of entryIndex and then return this index.
/// @param pageIndex if the memory is big array where each spot in it is in size PAGE_SIZE then this index indicates the spot where the page table we need is in
/// @param entryIndex the index in the page table table
/// @param isPageTableIndex a boolean that represent if the value index that is going to be in the page table in the spot entryIndex going to represent another 
/// page table or is it going to contain the index to page in memory that will be the memory that the user can access to
/// @param physicalPageIndex if the value isPageTableIndex is turned on then we would like to input this param physicalPageIndex 
/// into the page table in the entry index spot instead of what it currently has which is usually nothing
/// @param overrunTableEntry if the page index that we are going to get from page table in the entry index representing a page that the user can use its memory freely
/// and there is already value in there. the flag overrunTableEntry represents if we can overrun it or not
static inline uint64_t createOrFindNextPageIndex(uint64_t pageIndex, uint64_t entryIndex, uint64_t physicalPageIndex, bool isPageTableIndex, bool overrunTableEntry);


