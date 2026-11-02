/*============================================================================

  asm/fcntl.h

  Definition of the open() constants, but not the function itself 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

//Octal
#define O_RDONLY        00000 
#define O_WRONLY        00001 
#define O_RDWR          00002
#define O_CREAT         00100 
#define O_TRUNC         01000 
#define O_APPEND        02000 
#define O_DIRECTORY   0200000

