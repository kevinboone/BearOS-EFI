/*============================================================================

  defs.h

  General definitions

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef unsigned char BYTE;
typedef unsigned char UTF8;
typedef unsigned int UTF32;
// Define size_t, etc., because we can't include the standard C headers
typedef __SIZE_TYPE__ size_t;

