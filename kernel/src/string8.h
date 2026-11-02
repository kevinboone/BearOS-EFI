/*============================================================================

  string8.h

  A variable-length UTF8/ASCII string.  

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "list.h"

struct _String8;
typedef struct _String8 String8;

extern void string8_append (String8 *self, const UTF8 *s);
extern void string8_append_char (String8 *self, UTF32 c);
extern void string8_append_byte (String8 *self, BYTE c);
extern UTF8 *string8_cstr (const String8 *self);
extern void string8_destroy (String8 *self);
extern int string8_length (const String8 *self);
extern String8 *string8_new (const UTF8 *s);
extern String8 *string8_new_empty (void);

/* tokenize() splits a string into tokens at white spaces.
 * double-quotes prevent spaces splitting the line; an
 * escape \ before a space has the same effect. To get a quote
 * in the parsed output, use \". \" can also be used inside a 
 * quoted block -- "fred\"s" parses correctly with a " in the middle.
 * Parsing rules are similar to the shell, but not identical. In particular,
 * we don't support nested quotes
 * Caller must unref the list, which will always be valid
 * (but may be empty) */
List *string8_tokenize  (const UTF8 *line);

