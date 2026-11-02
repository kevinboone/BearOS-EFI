/*============================================================================

  pseudofshandler.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "list.h"
#include "fileops.h"

struct _PseudoFsHandler;
typedef struct _PseudoFsHandler PseudoFsHandler;

// entries is a list of UTF8 strings
typedef int (*PseudoFsPopulateFn) (List *entries); 
typedef struct FileOps * (*PseudoFsOpenFileFn) (const char *name);

extern void pseudofshandler_destroy (PseudoFsHandler *self); 

/** Return an FSOps structure appopriate for this particular handler.
    The caller must store and free it -- it is dynamically generated. */
extern struct FSOps *pseudofshandler_get_fsops (PseudoFsHandler *self); 

extern PseudoFsHandler *pseudofshandler_new (PseudoFsPopulateFn pop_fn, 
        PseudoFsOpenFileFn open_fn, const char *type, int32_t magic); 


