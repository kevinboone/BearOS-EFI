/*============================================================================

  bearos_term.h 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// These functions return -1 and set errno on error
int bearos_term_get_modes (int fd);
int bearos_term_get_current_mode (int fd);

// Get size of specified mode. Returns 0 on success
int bearos_term_get_mode_size (int fd, int mode, int *rows, int *cols);

// Set the current screen mode 
int bearos_term_set_mode (int fd, int mode);

#ifdef __cplusplus
}
#endif
