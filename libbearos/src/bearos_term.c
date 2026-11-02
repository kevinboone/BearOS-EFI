/*============================================================================

  term.c 

  BearOS helper functions for terminal management 

============================================================================*/

#include <asm/termbits.h>
#include <errno.h>
#include <libbearos/bearos_term.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>


/*============================================================================

 bearos_term_get_modes 

============================================================================*/
int bearos_term_get_modes (int fd)
  {
  int num_modes = 0;
  if (ioctl (fd, BEAROS_TERM_GET_NUM_MODES, (intptr_t)&num_modes, 0, 0) == 0)
    {
    return num_modes;
    }
  else
    return -1; // errno should have been set
  }


/*============================================================================

 bearos_term_get_current_mode

============================================================================*/
int bearos_term_get_current_mode (int fd)
  {
  int mode = 0;
  if (ioctl (fd, BEAROS_TERM_GET_CURRENT_MODE, (intptr_t)&mode, 0, 0) == 0)
    {
    return mode;
    }
  else
    return -1; // errno should have been set
  }


/*============================================================================

 bearos_term_get_size

============================================================================*/
int bearos_term_get_mode_size (int fd, int mode, int *rows, int *cols)
  {
  return ioctl (fd, BEAROS_TERM_GET_MODE_SIZE, (intptr_t)mode, 
      (intptr_t)rows, (intptr_t)cols); 
  }


/*============================================================================

 bearos_term_set_mode

============================================================================*/
int bearos_term_set_mode (int fd, int mode)
  {
  return ioctl (fd, BEAROS_TERM_SET_MODE, (intptr_t)mode, 0, 0); 
  }



