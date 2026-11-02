/*============================================================================

  stdint.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

typedef __INTMAX_TYPE__         intmax_t; 
typedef __INT64_TYPE__          int64_t; 
typedef __INT32_TYPE__          int32_t; 
typedef __INT16_TYPE__          int16_t; 
typedef __INT8_TYPE__           int8_t; 

typedef __UINTMAX_TYPE__        uintmax_t; 
typedef __UINT64_TYPE__         uint64_t; 
typedef __UINT32_TYPE__         uint32_t; 
typedef __UINT16_TYPE__         uint16_t; 
typedef unsigned char           uint8_t; 

typedef __INT_FAST8_TYPE__      int_fast8_t;

// In 64-bit arch, all "fast" types > 8 bits are _long_ (oddly)
typedef __INT_FAST16_TYPE__     int_fast16_t;
typedef __INT_FAST32_TYPE__     int_fast32_t;
typedef __INT_FAST64_TYPE__     int_fast64_t;

typedef __UINT_FAST8_TYPE__     uint_fast8_t;

// In 64-bit arch, all "fast" types > 8 bits are _long_ (oddly)
typedef __UINT_FAST16_TYPE__    uint_fast16_t;
typedef __UINT_FAST32_TYPE__    uint_fast32_t;
typedef __UINT_FAST64_TYPE__    uint_fast64_t;

/* Types for `void *' pointers.  */
typedef __INTPTR_TYPE__         intptr_t;
typedef __UINTPTR_TYPE__        uintptr_t;


#ifndef CHAR16
typedef uint16_t CHAR16;
#endif

