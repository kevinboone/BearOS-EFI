/*============================================================================

  ls utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <conio.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static char *months[12]; 

static int ls_file (const char *path, const char *orig_path);

/*============================================================================

  ls_entry

============================================================================*/
static int ls_entry (const char *path, const char *d_name)
  {
  int ret = 0;
  struct stat sb;
  if (stat (path, &sb) == 0)
    {
    if ((sb.st_mode & S_IFMT) == S_IFDIR)
          putchar ('d');
    else if ((sb.st_mode & S_IFMT) == S_IFCHR)
          putchar ('c');
    else if ((sb.st_mode & S_IFMT) == S_IFBLK)
          putchar ('b');
    else
          putchar ('-');

    if (sb.st_mode & S_IRUSR)
          putchar ('r');
    else
          putchar ('-');

    if (sb.st_mode & S_IWUSR)
          putchar ('w');
    else
          putchar ('-');

    if (sb.st_mode & S_IXUSR)
          putchar ('x');
    else
          putchar ('-');

    if (sb.st_mode & S_IRGRP)
          putchar ('r');
    else
          putchar ('-');

    if (sb.st_mode & S_IWGRP)
          putchar ('w');
    else
          putchar ('-');

    if (sb.st_mode & S_IXGRP)
          putchar ('x');
    else
          putchar ('-');

    if (sb.st_mode & S_IROTH)
          putchar ('r');
    else
          putchar ('-');

    if (sb.st_mode & S_IWOTH)
          putchar ('w');
    else
          putchar ('-');

    if (sb.st_mode & S_IXOTH)
          putchar ('x');
    else
          putchar ('-');

    printf (" %8d ", sb.st_size);

    struct tm *tm = gmtime (&sb.st_mtime);
    printf (" %s %02d %04d %02d:%02d ", months[tm->tm_mon], tm->tm_mday, 
          tm->tm_year + 1900, tm->tm_hour, tm->tm_min);
    }
  else
    ret = errno;

  printf ("%s\n", d_name);
  return ret;
  }


/*============================================================================

  ls_dir

============================================================================*/
static int ls_dir (const char *path, const char *orig_path)
  {
  int ret = 0;

  DIR *p = opendir (path);
  if (p)
    {
    struct dirent *dirp = readdir (p);
    while (dirp)
      {
      char newpath[PATH_MAX];
      strcpy (newpath, path);
      if (newpath[strlen (newpath) - 1] != '/')
        strcat (newpath, "/");
      strcat (newpath, dirp->d_name);

      ls_entry (newpath, dirp->d_name);

      dirp = readdir (p);
      }
    closedir (p);
    }
  else
    {
    printf ("%s: %s%s", path, strerror (errno), "\n");
    ret = errno;
    }

  return ret;
  }

/*============================================================================

  ls_path

============================================================================*/
int ls_path (const char *path)
  {
  int ret = 0;

  struct stat sb;
  if (stat (path, &sb) == 0)
    {
    if ((sb.st_mode & S_IFMT) == S_IFDIR)
      {
      ret = ls_dir (path, path);
      }
    else
      {
      ret = ls_file (path, path);
      }
    }
  else
    {
    printf ("%s: %s\n", path, strerror (errno));
    ret = errno;
    }

  return ret;
  }


/*============================================================================

  ls_dir

============================================================================*/
static int ls_file (const char *path, const char *orig_path)
  {
  int ret = ls_entry (path, orig_path);
  return ret;
  }


/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  months[0] = "Jan";
  months[1] = "Feb";
  months[2] = "Mar";
  months[3] = "Apr";
  months[4] = "May";
  months[5] = "Jun";
  months[6] = "Jul";
  months[7] = "Aug";
  months[8] = "Sep";
  months[9] = "Oct";
  months[10] = "Nov";
  months[11] = "Dec";

  int ret = 0;
  if (argc == 1)
    {
    char cwd[PATH_MAX];
    getcwd (cwd, PATH_MAX - 1);
    ret = ls_path (cwd);
    }
  else if (argc == 2)
    {
    const char *path = argv[1];
    ret = ls_path (path);
    }
  else
    {
    for (int i = 1; i < argc; i++)
      {
      const char *path = argv[i]; 
      printf ("%s:%s", path, "\n");
      ret = ls_path (path);
      }
    }
  return ret;
  }

