#ifdef DEBUG

#include "test.h"
#include "utility/utility.h"

void testEverything()
{
    testIntLength();
    testStrCmp();
    printf("current tests are successful\n");
}

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

#endif // DEBUG