/*============================================================================

  pseudofshandler.c

  This class is supposed to implement a general-purpose pseudo-device, for
  directories this /dev and /proc. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_dirent.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "compat_stdio.h"
#include "compat_string.h"
#include "devmgr.h"
#include "pseudofshandler.h"
#include "fsops.h"
#include "../../lib/include/errno_numbers.h"
#include "procmgr.h"
#include "../../lib/include/stddef.h"
#include "list.h"

struct _PseudoFsHandler
  {
  PseudoFsPopulateFn pop_fn;
  PseudoFsOpenFileFn open_fn;
  char type[32]; // e.g., procfs, devfs
  int32_t magic; // for use in fstat()
  };

typedef struct _PFHRootData
  {
  PseudoFsHandler *self;
  List *entries; // List of UTF8 strings
  int pos; // Position in entry list of next file read
  } PFHRootData;

// FWD
static struct FileOps *pseudofshandler_open (void *data, 
        const UTF8 *path, int mode);
static int pseudofshandler_root_close_and_cleanup (struct FileOps *fileops);
static ssize_t pseudofshandler_root_read (struct FileOps *fileops, 
    void *buff, size_t count);
static int pseudofshandler_root_fstat (struct FileOps *fileops, 
    struct stat *sb);
static int pseudofshandler_populate_root (PseudoFsHandler *self, 
    List *entries);
static int pseudofshandler_statfs (void *data, struct statfs *sb);
static int pseudofshandler_umount (void *data);

/*============================================================================

  pseudofshandler_destroy

============================================================================*/
void pseudofshandler_destroy (PseudoFsHandler *self)
  {
  free (self);
  }


/*============================================================================

  pseudofshandler_get_fsops

============================================================================*/
struct FSOps *pseudofshandler_get_fsops (PseudoFsHandler *self)
  {
  struct FSOps *fsops = malloc (sizeof (struct FSOps));
  memset (fsops, 0, sizeof (struct FSOps));
  fsops->data = self;
  fsops->open = pseudofshandler_open;
  fsops->statfs = pseudofshandler_statfs;
  fsops->umount = pseudofshandler_umount;
  strcpy ((char *)fsops->type, self->type);
  return fsops;
  }


/*============================================================================

  pseudofshandler_get_root_fileops

============================================================================*/
static struct FileOps *pseudofshandler_get_file_fileops (PseudoFsHandler *self, 
     const UTF8 *name)
  {
  return self->open_fn ((const char *)name);
  }

/*============================================================================

  pseudofshandler_get_root_fileops

============================================================================*/
static struct FileOps *pseudofshandler_get_root_fileops (PseudoFsHandler *self) 
  {
  struct FileOps *fileops = malloc (sizeof (struct FileOps));
  PFHRootData *data = malloc (sizeof (PFHRootData)); 
  data->entries = list_create ((ListFreeItemFn)free);
  data->pos = 0;
  // TODO check errors from creating root
  pseudofshandler_populate_root (self, data->entries);
  data->self = self;
  fileops->data = data;
  fileops->close_and_cleanup = pseudofshandler_root_close_and_cleanup;
  fileops->read = pseudofshandler_root_read;
  fileops->fstat = pseudofshandler_root_fstat;
  fileops->refs = 1;
  return fileops;
  }


/*============================================================================

  pseudofshandler_populate_root

============================================================================*/
static int pseudofshandler_populate_root (PseudoFsHandler *self, List *entries)
  {
  self->pop_fn (entries);
  return 0;
  }

/*============================================================================

  pseudofshandler_root_close_and_cleanup

============================================================================*/
static int pseudofshandler_root_close_and_cleanup (struct FileOps *fileops)
  {
  PFHRootData *data = fileops->data;
  if (data->entries) list_destroy (data->entries);
  free (data);
  return 0;  
  }


/*============================================================================

  pseudofshandler_root_fstat

============================================================================*/
static int pseudofshandler_root_fstat (struct FileOps *fileops, 
    struct stat *sb)
  {
  sb->st_mode = S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP 
    | S_IXGRP | S_IROTH | S_IXOTH;
  sb->st_size = 0;
  sb->st_mtime = 0;
  return 0;
  }

/*============================================================================

  pseudofshandler_root_read

============================================================================*/
static ssize_t pseudofshandler_root_read (struct FileOps *fileops, 
    void *buff, size_t count)
  {
  if (count < sizeof (struct dirent))
    {
    set_errno (EINVAL);
    return -1;
    }

  PFHRootData *root_data = (PFHRootData *)fileops->data; 

  List *entries = root_data->entries;

  int l = list_length (entries);
  if (root_data->pos >= l)
    {
    // We've read to the end of the list of files. Return 0 to indicate
    //   end-of-file on this pseudo-directory
    return 0;
    }

  // pos < length, so more to read
  UTF8 *entry = list_get (entries, root_data->pos);
  
  struct dirent *de = (struct dirent *)buff;
  strncpy (de->d_name, (char *)entry, sizeof (de->d_name));
  root_data->pos++;

  return sizeof (struct dirent);  
  }


/*============================================================================

  pseudofshandler_new

============================================================================*/
PseudoFsHandler *pseudofshandler_new (PseudoFsPopulateFn pop_fn, 
        PseudoFsOpenFileFn open_fn, const char *type, int32_t magic) 
  {
  PseudoFsHandler *self = malloc (sizeof (PseudoFsHandler));
  self->pop_fn = pop_fn;
  self->open_fn = open_fn;
  strncpy (self->type, type, sizeof (self->type) - 1);
  self->magic = magic;
  return self;
  }


/*============================================================================

  pseudofshandler_open

============================================================================*/
static struct FileOps *pseudofshandler_open (void *data, 
    const UTF8 *path, int mode)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  PseudoFsHandler *self = (PseudoFsHandler *)fsops->data;
  if (strcmp ((char *)path, "/") == 0)
    {
    struct FileOps *fileops = pseudofshandler_get_root_fileops (self); 
    return fileops;
    }
  else
    {
    path++; // Skip "/"
    struct FileOps *fileops = pseudofshandler_get_file_fileops (self, path); 
    return fileops;
    }
  }

/*============================================================================

  pseudofshandler_statfs

============================================================================*/
static int pseudofshandler_statfs (void *data, struct statfs *sb)
  {
  PseudoFsHandler *self = (PseudoFsHandler *)data;
  memset (sb, 0, sizeof (struct statfs)); 
  sb->f_type = self->magic;
  sb->f_bsize = 512; // Irrelevant, but we have to say something
  return 0;
  }


/*============================================================================

  pseudofshandler_umount

============================================================================*/
static int pseudofshandler_umount (void *data)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  PseudoFsHandler *self = (PseudoFsHandler *)fsops->data;
  pseudofshandler_destroy (self); 
  return 0;
  }


