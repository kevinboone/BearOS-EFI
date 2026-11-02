/*============================================================================

  string8.c

  A variable-length UTF8/ASCII string.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libbearos/bearos_defs.h>
#include <libbearos/string8.h>
#include <libbearos/list.h>

struct _String8
  {
  UTF8 *str;
  };

/*============================================================================

  string_tokenize_char16 

============================================================================*/

// Dunno state -- usually start of line where nothing has been read
#define STATE_DUNNO 0
// White state -- eating whitespace
#define STATE_WHITE 1
// General state -- eating normal chars
#define STATE_GENERAL 2
// dquote state -- last read was a double quote 
#define STATE_DQUOTE 3
// dquote state -- last read was a double quote 
#define STATE_ESC 4
// Comment state -- ignore until EOL
#define STATE_COMMENT 5
// General char -- anything except escape, quotes, whitespace
#define CHAR_GENERAL 0
// White char -- space or tab
#define CHAR_WHITE 1
// Double quote char 
#define CHAR_DQUOTE 2
// Double quote char 
#define CHAR_ESC 3
// Hash comment
#define CHAR_HASH 4

List *string8_tokenize (const UTF8 *s)
  {
  List *argv = list_create ((ListFreeItemFn) string8_destroy); 

  int i, l = strlen ((char *)s);

  String8 *buff = string8_new_empty();

  int state = STATE_DUNNO;
  int last_state = STATE_DUNNO;

  for (i = 0; i < l; i++)
    {
    UTF8 c = s[i];
    int chartype = CHAR_GENERAL;
    switch (c)
      {
      case ' ': case '\t':
        chartype = CHAR_WHITE;
        break;

      case '"':
        chartype = CHAR_DQUOTE;
        break;

      case '\\':
        chartype = CHAR_ESC;
        break;

      case '#':
        chartype = CHAR_HASH;
        break;

      default:
        chartype = CHAR_GENERAL;
      }

    //Print (L"char=%d type=%d\n", c, chartype);

    switch (1000 * state + chartype)
      {
      // --- Dunno states ---
      case 1000 * STATE_DUNNO + CHAR_GENERAL:
        string8_append_char (buff, c);
        state = STATE_GENERAL;
        break;

      case 1000 * STATE_DUNNO + CHAR_WHITE:
        // ws at start of line, or the first ws after
        if (string8_length (buff))
          list_append (argv, buff);
        buff = string8_new_empty();
        state = STATE_WHITE;
        break;

      case 1000 * STATE_DUNNO + CHAR_DQUOTE:
        // Eat the quote and go into dqote mode
        state = STATE_DQUOTE;
        break;

      case 1000 * STATE_DUNNO + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        break;

      case 1000 * STATE_DUNNO + CHAR_HASH:
        last_state = state;
        state = STATE_COMMENT;
        break;

      // --- White states ---
      case 1000 * STATE_WHITE + CHAR_GENERAL:
        // Got a char while in ws
        string8_append_char (buff, c);
        state = STATE_GENERAL;
        break;

      case 1000 * STATE_WHITE + CHAR_WHITE:
        // Eat ws
        break;

      case 1000 * STATE_WHITE + CHAR_DQUOTE:
        // Eat the ws and go into dqote mode
        state = STATE_DQUOTE;
        break;

      case 1000 * STATE_WHITE + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        break;

      case 1000 * STATE_WHITE + CHAR_HASH:
        last_state = state;
        state = STATE_COMMENT;
        break;

      // --- General states ---
      case 1000 * STATE_GENERAL + CHAR_GENERAL:
        // Eat normal char 
        string8_append_char (buff, c);
        break;

      case 1000 * STATE_GENERAL + CHAR_WHITE:
        //Hit ws while eating characters -- this is a token
        if (string8_length (buff))
          list_append (argv, buff);
        buff = string8_new_empty();
        state = STATE_WHITE;
        break;

      case 1000 * STATE_GENERAL + CHAR_DQUOTE:
        // Go into dquote mode 
        state = STATE_DQUOTE;
        break;

      case 1000 * STATE_GENERAL + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        break;

      case 1000 * STATE_GENERAL + CHAR_HASH:
        //Hit hash while eating characters -- this is a token
        if (string8_length (buff))
          list_append (argv, buff);
        buff = string8_new_empty();
        state = STATE_COMMENT;
        break;

       // --- Dquote states ---
      case 1000 * STATE_DQUOTE + CHAR_GENERAL:
        // Store the char, but remain in dquote mode
        string8_append_char (buff, c);
        break;

      case 1000 * STATE_DQUOTE + CHAR_WHITE:
        // Store the ws, and remain in dquote mode
        string8_append_char (buff, c);
        break;

      case 1000 * STATE_DQUOTE + CHAR_DQUOTE:
        // Leave duote mode and store token (which might be empty) 
        list_append (argv, buff);
        buff = string8_new_empty();
        state = STATE_DUNNO;
        break;

      case 1000 * STATE_DQUOTE + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        //string8_append_char (buff, c);
        break;

      case 1000 * STATE_DQUOTE + CHAR_HASH:
        // Keep this hash char -- it is quoted 
        string8_append_char (buff, c);
        break;

      // --- Esc states ---
      case 1000 * STATE_ESC + CHAR_GENERAL:
        string8_append_char (buff, c);
        state = STATE_GENERAL;
        break;

      case 1000 * STATE_ESC + CHAR_WHITE:
        string8_append_char (buff, c);
        state = last_state;
        break;

      case 1000 * STATE_ESC + CHAR_DQUOTE:
        string8_append_char (buff, '"');
        //state = last_state;
        state = STATE_DUNNO;
        break;

      case 1000 * STATE_ESC + CHAR_ESC:
        string8_append_char (buff, '\\');
        state = last_state;
        break;

      case 1000 * STATE_ESC + CHAR_HASH:
        string8_append_char (buff, '#');
        state = last_state;
        break;

      // --- Comment states ---
      case 1000 * STATE_COMMENT + CHAR_GENERAL:
        break;

      case 1000 * STATE_COMMENT + CHAR_WHITE:
        break;

      case 1000 * STATE_COMMENT + CHAR_DQUOTE:
        break;

      case 1000 * STATE_COMMENT + CHAR_ESC:
        break;

      case 1000 * STATE_COMMENT + CHAR_HASH:
        break;

      default:
        fprintf 
           (stderr, "Internal error: bad state %d and char type %d in tokenize()",
               state, chartype);
      }
    }

  if (buff)
    {
    if (string8_length (buff) > 0)
      {
      list_append (argv, buff);
      }
    else
      string8_destroy (buff);
    }

  return argv;
  }


/*============================================================================

  string8_append

============================================================================*/
void string8_append (String8 *self, const UTF8 *s)
  {
  int oldlen = strlen ((char *)self->str);
  int newlen = oldlen + strlen ((char *)s);
  int newmem = newlen + 1; // For the zero

  char *old = (char *)self->str;
  self->str = malloc (newmem);
  strcpy ((char *)self->str, old);
  strcpy ((char *)self->str + oldlen, (char *)s);
  free (old);
  }


/*============================================================================

  string8_append_byte

============================================================================*/
void string8_append_byte (String8 *self, BYTE c)
  {
  UTF8 buff[2];
  buff[0] = c;
  buff[1] = 0;
  string8_append (self, buff);
  }


/*============================================================================

  string8_append_char_char

============================================================================*/
void string8_append_char (String8 *self, UTF32 ch)
  {
 if (ch < 0x80)
    {
    string8_append_byte (self, (BYTE)ch);
    }
  else if (ch < 0x0800)
    {
    string8_append_byte (self, (BYTE)((ch >> 6) | 0xC0));
    string8_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  else if (ch < 0x10000)
    {
    string8_append_byte (self, (BYTE)((ch >> 12) | 0xE0));
    string8_append_byte (self, (BYTE)((ch >> 6 & 0x3F) | 0x80));
    string8_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  else
    {
    string8_append_byte (self, (BYTE)((ch >> 18) | 0xF0));
    string8_append_byte (self, (BYTE)(((ch >> 12) & 0x3F) | 0x80));
    string8_append_byte (self, (BYTE)(((ch >> 6) & 0x3F) | 0x80));
    string8_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  }


/*============================================================================

  string8_destroy

============================================================================*/
void string8_destroy (String8 *self)
  {
  if (self)
    {
    if (self->str)
      free (self->str);
    free (self);
    }
  }

/*============================================================================

  string8_cstr

============================================================================*/
UTF8 *string8_cstr (const String8 *self)
  {
  return self->str;
  }

/*============================================================================

  string8_length

============================================================================*/
int string8_length (const String8 *self)
  {
  return strlen ((char *)self->str);
  }


/*============================================================================

  string8_new

============================================================================*/
String8 *string8_new (const UTF8 *s)
  {
  String8 *self = malloc (sizeof (String8));
  int l = strlen ((char *)s);
  self->str = malloc ((l + 1) * sizeof (UTF8));
  strcpy ((char *)self->str, (char *)s); 
  return self;
  }


/*============================================================================

  string8_new_empty

============================================================================*/
String8 *string8_new_empty (void)
  {
  String8 *self = malloc (sizeof (String8));
  self->str = malloc (1);
  self->str[0] = 0;
  return self;
  }

/*============================================================================

  string8_split

============================================================================*/
List *string8_split (String8 *self, const char *delim)
  {
  List * l = list_create ((ListFreeItemFn)free);

  char *s = strdup ((char *)self->str);
  
  char *tok = strtok (s, delim);

  if (tok)
    {
    do
      {
      list_append (l, strdup (tok));
      } while ((tok = strtok (NULL, delim)));
    }

  free (s);

  return l;
  }


/*============================================================================

  string8_trim_left

============================================================================*/
void string8_trim_left (String8 *self)
  {
  const char *s = (const char *)self->str;
  int l = strlen (s);
  int i = 0;
  int pos = 0;
  BOOL stop = FALSE;
  while (i < l && !stop)
    {
    char c = s[i];
    if (c == ' ' || c == '\n' || c == '\t')
      {
      pos++;
      }
    else
      stop = TRUE;
    i++;
    }
  char *s_new = strdup (s + pos);
  free (self->str);
  self->str = (UTF8 *)s_new;
  }


/*============================================================================

  string8_trim_right

============================================================================*/
void string8_trim_right (String8 *self)
  {
  char *s = (char *)self->str;
  int l = strlen (s);
  int i = l - 1;
  BOOL stop = FALSE;
  while (i >= 0 && !stop)
    {
    char c = s[i];
    if (c == ' ' || c == '\n' || c == '\t')
      {
      s[i] = 0;
      }
    else
      stop = TRUE;
    i--;
    }
  }






