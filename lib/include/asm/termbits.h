/*============================================================================

  termbits.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#define TIOCGWINSZ 1000

// BearOS-specific ioctl numbers

// EFI terminal stuff
// Get number of console text modes
#define BEAROS_TERM_GET_NUM_MODES  10000
// Get current console mode 
#define BEAROS_TERM_GET_CURRENT_MODE  10001
// Get rows and cols for specified mode 
#define BEAROS_TERM_GET_MODE_SIZE  10002
// Set the terminal mode
#define BEAROS_TERM_SET_MODE 10003
// Get cursor visibility 
#define BEAROS_TERM_GET_CURSOR 10004
// Set cursor visibility 
#define BEAROS_TERM_SET_CURSOR 10005


