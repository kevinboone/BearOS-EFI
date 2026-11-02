/*============================================================================

  console.h

  Functions for managing the console 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

/** Clear the screen. */
extern void console_clear (void);

/** Returns zero if a line is read, even an empty one; -1 if the read is
    interrupted (ie., by ctrl-c) */
extern int console_readline (UTF8 *buff, int bufflen);
