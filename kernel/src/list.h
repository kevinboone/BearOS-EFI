/*============================================================================

  list.h

  A linked list.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

struct List;
typedef struct _List List;

typedef void (*ListFreeItemFn)(void *);

extern void  list_append (List *self, void *item);
extern List *list_create (ListFreeItemFn fn);
extern void  list_destroy (List *self);
extern void *list_get (const List *self, int n);
extern int   list_length (const List *self);
extern void  list_remove_at (List *self, int index);

/** Remove the specific item from the list, if it is present. The object's
free function will be called. This method can't be used to remove an
object by value -- that is, you can't pass "dog" to the method to remove
all strings whose value is "dog". */ 
extern void  list_remove_object (List *self, const void *item);


