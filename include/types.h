#ifndef H_TYPES
#define H_TYPES

#define NULL ((void *)0UL)

#ifndef BOOTLOADER
typedef unsigned long uintptr_t;
#else
typedef unsigned short uintptr_t;
#endif
typedef unsigned long size_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef uint8_t byte;

typedef uint32_t addr; 

#define CHAR_BIT 8
#define INT32_MAX 0x7fffffffL
#define UINT32_MAX 0xffffffffL

// bool
#ifndef bool
#define bool uint8_t
#define true 1
#define false 0
#endif

#endif