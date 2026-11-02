/*============================================================================

  list.c

  A linked list. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <stdlib.h> 
#include <libbearos/bearos_defs.h> 
#include <libbearos/list.h> 

typedef struct _ListItem
  {
  struct _ListItem *next;
  void *data;
  } ListItem;

struct _List
  {
  ListFreeItemFn free_fn;
  ListItem *head;
  };

/*============================================================================

  list_append

============================================================================*/
void list_append (List *self, void *item)
  {
  ListItem *i = malloc (sizeof (ListItem));
  i->data = item;
  i->next = 0;
  
  if (self->head)
    {
    ListItem *l = self->head;
    while (l->next)
      l = l->next;
    l->next = i;
    }
  else
    {
    self->head = i;
    }

  }

/*============================================================================

  list_create

============================================================================*/
List *list_create (ListFreeItemFn fn)
  {
  List *self = malloc (sizeof (List));
  self->free_fn = fn;
  self->head = 0;
  return self;
  }


/*============================================================================

  list_destroy

============================================================================*/
void list_destroy (List *self)
  {
  if (self) 
    {
    ListItem *l = self->head;
    while (l)
      {
      if (self->free_fn)
        self->free_fn (l->data);
      ListItem *temp = l;
      l = l->next;
      free (temp);
      }

    free (self);
    }
  }


/*============================================================================

  list_get

============================================================================*/
void *list_get (const List *self, int n)
  {
  ListItem *l = self->head;
  int i = 0;
  while (l != 0 && i != n)
    {
    l = l->next;
    i++; 
    }
  return l->data;
  }


/*============================================================================

  list_length

============================================================================*/
int list_length (const List *self)
  {
  ListItem *l = self->head;

  int i = 0;
  while (l != 0)
    {
    l = l->next;
    i++;
    }

  return i;
  }





