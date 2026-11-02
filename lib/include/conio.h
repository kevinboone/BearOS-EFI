/*============================================================================

  conio.h 

  Some console functions that are specific to BearOS on EFI. These are to be
  used by client programs, not by the kernel

============================================================================*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Write directly to the EFI kernel. Note that unicode translation may
//   not yet be fully implemented. The output is not subject to
//   redirection.
void console_puts (char *s);

#ifdef __cplusplus
}
#endif

