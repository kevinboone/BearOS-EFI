/*============================================================================

  console.c

  Functions for managing the console.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <stdio.h>

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
int system (const char *);
#else
#include <efi/efi.h>
#include <efi/efilib.h>
#endif 

#include "defs.h" 
#include "console.h" 
#include "compat_string.h" 


/*============================================================================

  console_clear

============================================================================*/
void console_clear (void)
  {
#ifdef LINUX
  system ("clear"); 
#else
  uefi_call_wrapper (ST->ConOut->ClearScreen, 1, ST->ConOut);
#endif
  }

/*============================================================================

  console_readline

============================================================================*/
int console_readline (UTF8 *buff, int bufflen)
  {
  #ifdef LINUX
  long unsigned int len = bufflen;
  getline ((char **)&buff, &len, stdin);
  len = strlen ((char *)buff);
  buff[len - 1] = 0; 
  return 0;
  #else
  EFI_INPUT_KEY key;
  UINTN index; 
  int i = 0;
  buff[0] = 0;

  while (1)
    {
    uefi_call_wrapper (BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
    uefi_call_wrapper (ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    CHAR16 chr = key.UnicodeChar;
    //Print(L"key %d %d sod\n", key.ScanCode, chr);

    if (chr == 13) 
      {
      // CR
      Print(L"\n");
      return 0;
      }
    else if (chr == 3)
      {
      // Ctrl-C
      buff[0] = 0;
      return -1;
      }
    else if (chr == 8 || key.ScanCode == 8) 
      {
      // BS or DEL
      if (i > 0)
        {
        i--; // TODO : UTF8
        buff[i] = 0;
        Print(L"%c", 8);
        }
      }
    else if (i < bufflen - 1) 
      {
      // Anything else
      // TODO UTF16 to UTF8
      buff[i] = chr & 0xFF;
      i++;
      buff[i] = 0;
      Print(L"%c", chr);
      }

    //Print(L"key %d %d sod\n", key.ScanCode, chr);
    }
  #endif
  }


