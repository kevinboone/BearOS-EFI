/*============================================================================

  list.h

  A linked list.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

struct List;
typedef struct _List List;

typedef void (*ListFreeItemFn)(void *);

extern void list_append (List *self, void *item);
extern List *list_create (ListFreeItemFn fn);
extern void list_destroy (List *self);
extern void *list_get (const List *self, int n);
extern int list_length (const List *self);


