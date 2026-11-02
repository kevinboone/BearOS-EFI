/*============================================================================

  stddefs.h 

  BearOS alternative to the conventional stddefs.h

============================================================================*/

#pragma once

#ifndef NULL
#define NULL 0
#endif 

typedef __SIZE_TYPE__           size_t; 
typedef __PTRDIFF_TYPE__        ssize_t; 
typedef __PTRDIFF_TYPE__	ptrdiff_t; 
typedef ssize_t                 off_t;
