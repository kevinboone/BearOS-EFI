/*============================================================================

  BOSH shell utility

  script.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

extern int script_load_and_run (const char *argv0, const char *name);

/** *quit is set TRUE if the script does something that should shut
    down the shell. */
extern int script_parse_and_run_command (const char *line, BOOL *quit);

