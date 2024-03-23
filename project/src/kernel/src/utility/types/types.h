#pragma once 

// unsigned types:
typedef unsigned char      uint8_t ;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long size_t  ;

// singed types:
typedef char       int8_t ;
typedef short      int16_t;
typedef int        int32_t;
typedef long long  int64_t;

#define NULL 0
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// this macro returns 1 for positive value and -1 for negative value
#define BOOL_TO_INT(value) ((!value * -2) + 1)

#define MAX_UINT64 0xFFFFFFFFFFFFFFFF