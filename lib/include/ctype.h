/*============================================================================

  ctype.h 

  BearOS alternative to the conventional ctype.h

============================================================================*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Note atoi and atol are equivalent -- both are 32-bit values
int isdigit (int c);
int isxdigit (int c);
int isspace (int c);

#ifdef __cplusplus
}
#endif


