#pragma once

#ifdef DEBUG

void testEverything();

// testing utility functions:
static void testIntLength();
static void testStrCmp();
static void testHexToString();

// testing virtual memory functions:
static void testMapping();
static void testPageFault();

#endif