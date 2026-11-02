/*============================================================================

  BOSH shell utility

  script.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libbearos/bearos_defs.h>
#include <libbearos/bearos_term.h>
#include <libbearos/bearos_exec.h>
#include <libbearos/list.h>
#include <libbearos/path.h>
#include <libbearos/string8.h>
#include "script.h" 
#include "cmds.h" 
#include "exec.h" 

/*============================================================================

  script_load_and_run

============================================================================*/
int script_load_and_run (const char *argv0, const char *name)
  {
  int ret = 0;

  // TODO -- use $PATH?
  FILE *f = fopen (name, "r");
  if (f)
    {
    char *line = NULL;
    size_t n;
   
    BOOL quit = FALSE;

    int r = getline (&line, &n, f);
    while (r > 0 && !quit) 
      {
      String8 *line_s = string8_new ((UTF8 *)line); 
      string8_trim_right (line_s);
      string8_trim_left (line_s);
      const char *trimmed_line = (const char *)string8_cstr(line_s);
      if (string8_length (line_s) > 0 && trimmed_line[0] != '#')
        {
        ret = script_parse_and_run_command (trimmed_line, &quit);  
        }

      string8_destroy (line_s);
      if (line) free (line);
      line = NULL;
      r = getline (&line, &n, f);
      };

    if (line) free (line);

    fclose (f);
    }
  else
    {
    fprintf (stderr, "%s: can't run script: %s\n", name, strerror (errno));
    ret = -1;
    }

  return ret; 
  }


/*============================================================================

  script_parse_and_run_command

============================================================================*/
int script_parse_and_run_command (const char *line, BOOL *quit)
  {
  int ret;
  List *args = string8_tokenize ((UTF8 *)line); 
  int argc = list_length (args);
  if (argc > 0)
    {
    char **argv = malloc ((argc + 1) * sizeof (char *));
    for (int i = 0; i < argc; i++)
          argv[i] = strdup ((char *)string8_cstr 
            ((String8 *)list_get (args, i)));
    argv[argc] = NULL;
    const char *cmd = argv[0]; 
    if (strcmp (cmd, "exit") == 0)
      { 
      ret = 0;
      *quit = TRUE; 
      }
    else if (strcmp (cmd, "cd") == 0)
      ret = cmd_cd (argc, argv);
    else
      {
      ret = exec_run_command (cmd, argv);
      }
    for (int i = 0; i < argc; i++)
      free (argv[i]);
    free (argv);
    }
 else
    {
    // Nothing to do
    }

  list_destroy (args);
  return ret;
  }

