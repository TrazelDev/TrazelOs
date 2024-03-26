#include "test.h"
#include "utility/utility.h"
#include "src/memory/virtualMemory/virtualMemoryManager.h"
#include "src/memory/physicalMemory/pageFrameAllocator.h"

#ifdef DEBUG

void testEverything()
{
    // testing utility functions:
    testIntLength();
    testStrCmp();
    testHexToString();

    // testing virtual and physical memory:
    testMapping();
    testTranslation();
    testUnmapMemory();
    testReadPageFault();
    testWritePageFault();

    DEBUG_LOG("current tests are successful\n");
}

// testing utility functions:
static void testIntLength()
{
    ASSERT(getIntLength(-5) == 2);
    ASSERT(getIntLength(-5555) == 5);
    ASSERT(getIntLength(444444444) == 9);
    
    uint64_t value = -444444444000;
    ASSERT(getIntLength(value) == 13);
}
static void testStrCmp()
{
    ASSERT(strCmp("value\0", "value"));
    ASSERT(!strCmp("hello world", " hello world"));
}
static void testHexToString()
{
    ASSERT(strCmp("0x5555", hexToString((uint64_t)0x5555)));
    ASSERT(strCmp("0x5AEFC8D", hexToString((uint64_t)0x5AEFC8D)));
}


// testing virtual memory functions:
#define TEST_VALUE 0x5200
static void testMapping()
{
    PhysicalAddress pAddr1 = requestUserPage();
    PhysicalAddress pAddr2 = requestUserPage();
    VirtualAddress vAddr1;
    VirtualAddress vAddr2;

    vAddr1.raw = pAddr1.raw;
    vAddr2.raw = pAddr2.raw;

    // mapping the two address:
    ASSERT_PRINT_ERROR(mapMemory(pAddr1, vAddr1), printf("Error: mapping failed\n"))
    ASSERT_PRINT_ERROR(mapMemory(pAddr1, vAddr2), printf("Error: mapping failed\n"))


    // testing that the mapping actually works:
    *(vAddr1.uint64Ptr) = TEST_VALUE;
    ASSERT_PRINT_ERROR(*(vAddr2.uint64Ptr) == TEST_VALUE,
        printf("Error: virtual address and physical address mappings test failed\n"));
}
static void testTranslation()
{
    PhysicalAddress pAddr = requestUserPage();
    VirtualAddress  vAddr;
    vAddr.raw = pAddr.raw;
    mapMemory(pAddr, vAddr);
    ASSERT(getPhysicalAddress(vAddr).raw == pAddr.raw);
}
static void testUnmapMemory()
{
    PhysicalAddress pAddr = requestUserPage();
    VirtualAddress  vAddr;
    vAddr.raw = pAddr.raw;
    mapMemory(pAddr, vAddr);
    
    pAddr.raw += PAGE_SIZE;
    ASSERT(!mapMemory(pAddr, vAddr));
    unmapMemory(vAddr);
    ASSERT(mapMemory(pAddr, vAddr));
}
static void testReadPageFault()
{
    PhysicalAddress pAddr = requestUserPage();
    int value = *(pAddr.uint64Ptr);
    // if does not crush everything is okay
}
static void testWritePageFault()
{
    PhysicalAddress pAddr = requestUserPage();
    *(pAddr.uint64Ptr) = TEST_VALUE;
    ASSERT(*(pAddr.uint64Ptr) == TEST_VALUE);
}
#undef TEST_VALUE

#endif // DEBUG