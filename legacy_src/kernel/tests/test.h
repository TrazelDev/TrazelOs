#pragma once

#ifdef DEBUG

void testEverything();

// testing utility functions:
static void testIntLength();
static void testStrCmp();
static void testHexToString();

// testing virtual memory functions:
static void testMapping();
static void testTranslation();
static void testUnmapMemory();
static void testOverrideMapping();
static void testReadPageFault();
static void testWritePageFault();
static void testMemoryRequestor();

// testing malloc and free:
void testMallocAndFree();


#endif