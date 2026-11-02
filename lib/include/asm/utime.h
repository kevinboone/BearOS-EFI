/*============================================================================

  asm/utime.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

struct utimbuf
  {
  //time_t actime;
  // BearOS does not implement access time yet
  time_t modtime;
  };


