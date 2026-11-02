/*============================================================================

  text.h

  General text formatting functions 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

/** Convert a size in bytes into kB, Mb, etc, for ease of display. */
extern void text_humanize_size (long int n, char *buffer, int max);

