/*============================================================================

  dirent.h 

  dirent structures and constants (but not functions).

============================================================================*/

#pragma once

/** Our struct dirent is much simpler than the Linux one. */
struct dirent 
  {
  char d_name[256]; // Filename
  }; 



