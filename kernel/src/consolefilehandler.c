/*============================================================================

  consolefilehandler.c

  A provider of `struct fileops` for the console. On EFI, we'll use the
  EFI API to write directly to the console. On Linux, we'll just forward
  to the `write()` and `read()` implementations in the standard library.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifndef LINUX
#include <efi/efi.h>
#include <efi/efilib.h>
#endif

#ifdef LINUX
#include "../../lib/include/stddef.h"
//#include <stdio.h>
#include <termios.h>
struct termios old_tio;
ssize_t read (int fd, void *buff, size_t count);
#endif

#include "../../lib/include/asm/termbits.h"
#include "../../lib/include/asm/termios.h"
#include "../../lib/include/errno_numbers.h"
#include "compat_malloc.h"
#include "compat_stdio.h"
#include "compat_unistd.h"
#include "consolefilehandler.h"
#include "efiutil.h"
#include "fileops.h"
#include "process.h"
#include "procmgr.h"

// Bear in mind that in buffered mode, back-spacing won't work 
//   when the buffer is full. There's no way to take back
//   characters that the client code has already read. 
#define INPUT_BUFF_LEN 1024 

// Largest UTF8 sequence we're willing to handle
#define MAX_MB_CHARS 4

#ifdef LINUX
#ifndef CHAR16
#define CHAR16 UTF8
#endif
#endif

extern time_t global_boot_time;

struct _ConsoleFileHandler
  {
  BOOL buffered;
  BOOL echo;
  CHAR16 input_buff[INPUT_BUFF_LEN];
  int in_head;
  int in_tail;
  BOOL buffer_ready;

  // For converting UTF8 to EFI character format:
  UTF8 buff[MAX_MB_CHARS]; // Character we're working on
  int pos; // Where we are in the buffer
  int expect; // How many characters should follow the lead byte
  };


#ifndef LINUX
static void console_file_handler_output_char 
    (ConsoleFileHandler *self, UTF8 c);
#endif

/*============================================================================

  FWD 

============================================================================*/
int console_file_handler_struct_close_and_cleanup (struct FileOps *fs);
int console_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff);
int console_file_handler_struct_ioctl (struct FileOps *fs, int fd, 
        long request, intptr_t arg1, intptr_t args, intptr_t arg3);
ssize_t console_file_handler_struct_read (struct FileOps *fs, 
        void *buff, size_t count);
ssize_t console_file_handler_struct_write (struct FileOps *fs, 
        const void *buff, size_t count);


/*============================================================================

  consolefilehandler_close_and_cleanup

============================================================================*/
void consolefilehandler_close_and_cleanup 
                              (ConsoleFileHandler *self)
  {
  consolefilehandler_destroy (self);
  }


/*============================================================================

  consolefilehandler_destroy

============================================================================*/
void consolefilehandler_destroy (ConsoleFileHandler *self)
  {
  free (self);
  }


/*============================================================================

  consolefilehandler_get_fileops

============================================================================*/
struct FileOps *consolefilehandler_get_fileops (ConsoleFileHandler *self)
  {
  struct FileOps *fs = malloc (sizeof (struct FileOps));
  fs->refs = 1; // ??? 
  fs->data = self;
  fs->close_and_cleanup = console_file_handler_struct_close_and_cleanup;
  fs->fstat = console_file_handler_struct_fstat;
  fs->ioctl = console_file_handler_struct_ioctl;
  fs->read = console_file_handler_struct_read;
  fs->write = console_file_handler_struct_write;
  return fs;
  }


/*============================================================================

  consolefilehandler_new

============================================================================*/
ConsoleFileHandler *consolefilehandler_new (void)
  {
  ConsoleFileHandler *self = malloc (sizeof (ConsoleFileHandler));
  self->pos = 0;
  self->expect = 0;
  self->buffered = TRUE;
  self->echo = TRUE;
  self->in_head = 0;
  self->in_tail = 0;
  self->input_buff[0] = 0;
  self->buffer_ready = FALSE;
#ifdef LINUX
  tcgetattr (STDIN_FILENO, &old_tio);
  struct termios new_tio = old_tio;
  new_tio.c_lflag &=(~ICANON);
  tcsetattr (STDIN_FILENO,TCSANOW,&new_tio);

#else
  uefi_call_wrapper (ST->ConOut->EnableCursor, 2, ST->ConOut, 1);
#endif
  return self;
  }

/*============================================================================

  consolefilehandler_new

============================================================================*/
int console_file_handler_struct_close_and_cleanup (struct FileOps *fs)
  {
  // Not sure what, if anything, to do here
  //ConsoleFileHandler *self = (ConsoleFileHandler *)fs->data;
  //consolefilehandler_destroy (self);
  return 0;
  }

/*============================================================================

  console_file_handler_get_byte_count

============================================================================*/
#ifndef LINUX
static int console_file_handler_get_byte_count (UTF8 c)
  {
  if (c <= (UTF8)0x7F /* 01111111 */) return 1;
  else if (c <= (UTF8)0xBF /* 10111111 */) return -1;
  else if (c <= (UTF8)0xDF /* 11011111 */) return 2;
  else if (c <= (UTF8)0xEF /* 11101111 */) return 3;
  else if (c <= (UTF8)0xF7 /* 11110111 */) return 4;
  return -1; 
  }
#endif

/*============================================================================

  console_file_handler_input_char_buffered

============================================================================*/
#ifndef LINUX
static UTF8 console_file_handler_input_char_buffered
    (ConsoleFileHandler *self)
  {
  if (self->buffer_ready)
    {
    //myprintf ("NOW IN BUFFER head=%d tail=%d\n", self->in_head, self->in_tail);
    CHAR16 c = self->input_buff[self->in_tail];
    self->in_tail++; 
    if (self->in_tail == INPUT_BUFF_LEN) 
      self->in_tail = 0;
    if (self->in_tail == self->in_head)
      self->buffer_ready = FALSE;
    return c & 0xFF; // TODO
    }
 
  self->buffer_ready = FALSE;

  // Buffer is either empty, or not ready
  while (!self->buffer_ready)
    {
    UINTN index; 
    EFI_INPUT_KEY key;
    uefi_call_wrapper (BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
    uefi_call_wrapper (ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    UTF8 c = key.UnicodeChar & 0xFF; // TODO TODO TODO

    // Turn del into BS
    if (c == 0)
      { 
      if (key.ScanCode == 8)
	c = 8;
      else
	c = '?';
      }
  
    if (c == 8)
      {
      if (self->in_head != self->in_tail)
        {
        console_file_handler_output_char (self, c); // TODO TODO
	if (self->in_head == 0)
	  self->in_head = INPUT_BUFF_LEN - 1;
	else
	  self->in_head--;
        }
      }
    else
      {
      if (c == 13) 
	{
	self->buffer_ready = TRUE;
	c = 10;
        if (self->echo)
	  console_file_handler_output_char (self, 10); 
	}
      else if (c == 4) // Ctrl D
        {
        if (self->in_head == self->in_tail) 
          {
          self->in_head = 0; self->in_tail = 0;
          return 0;
          }
        else
          continue;
        }

      if (self->echo && c != 10)
        console_file_handler_output_char (self, c);

      self->input_buff[self->in_head] = c;

      self->in_head++;
      if (self->in_head >= INPUT_BUFF_LEN)
	{
	//self->buffer_ready = TRUE;
	self->in_head = 0;
	}

      //myprintf ("head=%d tail=%d c=%d\n", self->in_head, self->in_tail, c);

    if (self->in_head == self->in_tail)
      {
      self->buffer_ready = TRUE;
      //myprintf ("buffer ready\n");
      }

    /*
    */
      }

    //myprintf ("exist LOOP head=%d tail=%d\n", self->in_head, self->in_tail);
    UTF8 cc = console_file_handler_input_char_buffered (self);
    //myprintf ("got char %d\n");
    return cc;
    }
  return 0; // We should not get here
  }
#endif


/*============================================================================

  console_file_handler_input_char

============================================================================*/
#ifndef LINUX
static UTF8 console_file_handler_input_char 
    (ConsoleFileHandler *self)
  {
  if (self->buffered)
    return console_file_handler_input_char_buffered (self);
  else
    {
    UINTN index; 
    EFI_INPUT_KEY key;
    uefi_call_wrapper (BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
    uefi_call_wrapper (ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    UTF8 c = key.UnicodeChar & 0xFF; // TODO TODO TODO

    // Turn del into BS
    if (c == 0)
      { 
      if (key.ScanCode == 8)
	c = 8;
      else
	c = '?';
      }
    return c; // TODO
    }
  }
#endif


/*============================================================================

  console_file_handler_fstat

============================================================================*/
int console_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff)
  {
  statbuff->st_mode = S_IFCHR | 0666;
  statbuff->st_size = 0;
  statbuff->st_mtime = global_boot_time;
  return 0;
  }

/*============================================================================

  console_file_handler_ioctrl

============================================================================*/
int console_file_handler_struct_ioctl (struct FileOps *fs, int fd, 
       long request, intptr_t arg1, intptr_t arg2, intptr_t arg3)
  {
  set_errno (0);
  switch (request)
    {
#ifndef LINUX
    case TIOCGWINSZ:
      int mode = ST->ConOut->Mode->Mode; // Current text mode
      UINTN cols = 80, rows = 25;
      uefi_call_wrapper (ST->ConOut->QueryMode, 4, 
        ST->ConOut, mode, &cols, &rows);
      struct winsize *ws = (struct winsize *)arg1;
      ws->ws_row = rows;
      ws->ws_col = cols;
      return 0;

    case BEAROS_TERM_GET_NUM_MODES:
      int modes = ST->ConOut->Mode->MaxMode; 
      int *p = (int *)arg1;
      *p = modes;
      return 0;

    case BEAROS_TERM_GET_CURRENT_MODE:
      mode = ST->ConOut->Mode->Mode; 
      p = (int *)arg1;
      *p = mode;
      return 0;

    case BEAROS_TERM_GET_MODE_SIZE:
      mode = arg1; 
      cols = 80, rows = 25;
      int *r = (int *)arg2;
      int *c = (int *)arg3;
      EFI_STATUS status = uefi_call_wrapper (ST->ConOut->QueryMode, 4, 
          ST->ConOut, mode, &cols, &rows);
      if (status == EFI_SUCCESS)
        {
        *c = cols;
        *r = rows;
        return 0;
        }
     else
        {
        set_errno (efi_util_status_to_errno (status));
        return -1;
        }

    case BEAROS_TERM_SET_MODE:
      mode = arg1; 
      status = uefi_call_wrapper (ST->ConOut->SetMode, 2, 
          ST->ConOut, mode);
      if (status == EFI_SUCCESS)
        return 0;
     else
        {
        int e = efi_util_status_to_errno (status);
        set_errno (e);
        return -1;
        }

    case BEAROS_TERM_GET_CURSOR:
      int visible = ST->ConOut->Mode->CursorVisible; 
      p = (int *)arg1;
      *p = visible;
      return 0;

    case BEAROS_TERM_SET_CURSOR:
      int f = arg1;
      uefi_call_wrapper (ST->ConOut->EnableCursor, 2, 
          ST->ConOut, f);
      return 0;

#endif
    default: 
      set_errno (EINVAL);
      return -1;
    }
  }


/*============================================================================

  console_file_handler_mb_ti_efi

============================================================================*/
#ifndef LINUX
static CHAR16 console_file_handler_mb_to_efi (UTF8 *buff, int count)
  {
  int32_t cp;
  switch (count)
    {
    case 1:
      cp = buff[0];
      break;
    case 2:
      cp = (((unsigned)(buff[0] & 0x1F)) << 6)
	  | ((unsigned)(buff[1] & 0x3F));
      break;
    case 3:
      cp = (((unsigned)(buff[0] & 0x0F)) << 12)
	  | (((unsigned)(buff[1] & 0x3F)) << 6)
	  | ((unsigned)(buff[2] & 0x3F));
      break;
    case 4:
      cp = (((unsigned)(buff[0] & 0x07)) << 18)
	  | (((unsigned)(buff[1] & 0x3F)) << 12)
	  | (((unsigned)(buff[2] & 0x3F)) << 6)
	  | ((unsigned)(buff[3] & 0x3F));
      break;
    default:
    }

  return cp;
  }
#endif

/*============================================================================

  console_file_handler_handle_output_one_char
  Print a single CHAR16, making basic translations as necessary.

============================================================================*/
#ifndef LINUX
static void console_file_handler_output_one_char (CHAR16 c)
  {
  // Translate LF->CRLF
  if (c == 10)
    console_file_handler_output_one_char (13);

  // So far as I know, there is no single-character output function
  //   in the EFI API -- we have to compose a string
  CHAR16 t[2];
  t[0] = c;
  t[1] = 0; 
  uefi_call_wrapper (ST->ConOut->OutputString, 2, ST->ConOut, t);
  }
#endif

/*============================================================================

  console_file_handler_handle_char

============================================================================*/
#ifndef LINUX
static void console_file_handler_output_char 
    (ConsoleFileHandler *self, UTF8 c)
  {
  // Convert UTF8 to EFI UTF16. It's not real UTF16, and some high-numbered
  //   code points won't fit.

  int count = console_file_handler_get_byte_count (c);
  if (count == 1)
    {
    // This is the first and only byte of a sequence (or an error). Flush out
    //   the previous bytes in the buffer, in the hope that they
    //    make a complete sequence, then print this new one.

    if (self->expect > 1)
      {
      // We're not going to store a one-byte sequence in the buffer for long.
      // So only flush buffer if there's a multi-byte sequence in there.
      CHAR16 c = console_file_handler_mb_to_efi (self->buff, self->expect);
      console_file_handler_output_one_char ((CHAR16)c);
      }

    // Print this new character, then reset the buffer
    self->expect = 1;
    self->buff[0] = c;
    CHAR16 c = console_file_handler_mb_to_efi (self->buff, self->expect);
    console_file_handler_output_one_char ((CHAR16)c);
    self->pos = 0;
    self->expect = 0;
    }
  else
    {
    // This is either part of an existing multi-byte sequence,
    //   or the start of a new one.  
    self->buff[self->pos] = c;;
    self->pos++;
    if (self->expect > 1)
      {
      // So this is part of an existing sequence. Is it now complete?
      if (self->pos == self->expect)
        {
        // Sequence is complete -- store and flush
        CHAR16 c = console_file_handler_mb_to_efi (self->buff, self->expect);
        console_file_handler_output_one_char ((CHAR16)c);
        self->pos = 0;
        self->expect = 0;
        }
      else
        {
        // Sequence is not complete. We already stored the
        //   char, so nothing more to do. 
        }
      }
    else 
      {
      // We aren't expecting anything -- start of a new sequence
      self->buff[0] = c; 
      self->pos = 1;
      self->expect = count;
      }
    }
  }
#endif

/*============================================================================

  consolefilehandler_write

  We'd better hope there's a whole UTF8 character sequence at the end of
  the buffer, or the logic will overrun.

============================================================================*/
ssize_t console_file_handler_struct_write (struct FileOps *fs, 
    const void *buff, size_t len)
  {
#ifdef LINUX 

  // This sucks hard, but we'll only be here for testing. There's no obvious
  //   way to output a fixed number of bytes when building for Linux (because
  //   all the calls that ought to be able to, are stubbed out). So we'll
  //   write a nasty 0 on the end of the buffer. Ugh.
  char *nasty = (char *)buff;
  nasty[len] = 0;
  myprintf ("%s", nasty);

#else

  UTF8 *s = (UTF8 *)buff;
  for (size_t i = 0; i < len; i++)
    {
    UTF8 c = s[i];
    console_file_handler_output_char ((ConsoleFileHandler *)fs->data, c);
    }

#endif

  return 0;
  }


/*============================================================================

  consolefilehandler_write

  We'd better hope there's a whole UTF8 character sequence at the end of
  the buffer, or the logic will overrun.

============================================================================*/
ssize_t console_file_handler_struct_read (struct FileOps *fs, 
    void *buff, size_t len)
  {
  int ret;

#ifdef LINUX 
  ret = read (0, buff, len);
#else

  UTF8 *s = (UTF8 *)buff;
  int nread = 0;
  for (size_t i = 0; i < len; i++)
    {
    UTF8 c = console_file_handler_input_char ((ConsoleFileHandler *)fs->data);
    if (c != 0) 
      {
      nread++;
      s[i] = c;
      }
    }
  ret = nread;

#endif
  return ret;
  }



