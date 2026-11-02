/*============================================================================

  start.c

  This is the start-up code for the programs invoked by the ELF loader. 
  It must be linked with these programs and it _MUST_ be the first thing
  linked. The program loader will make a subroutine call to the 
  address of start().

============================================================================*/

#include <stdint.h> 
#include <stddef.h> 
#include "bearos/apidefs.h"

int main (int argc, char **argv);
extern int __bss_start__, __bss_end__;
char **environ;
BearosContext *bearos_context;

void stdio_init();
void stdio_cleanup();
int errno;

/*============================================================================

  Execution of a BearOS program starts here

============================================================================*/
int start (BearosContext *context, int argc, char **argv, char **envp)
  {
  // Blank the BSS section.
  // See https://kevinboone.me/gcc_bss.html for why
  for (char *p = (char*)&__bss_start__; p < (char*)&__bss_end__; p++) *p = 0;

  int ret = 0;

  bearos_context = context;
  environ = envp;

  // We'll store a reference to this point in the program, so the exit() call
  //   can jump back here and escape. Note that __builtin_setjmp does 
  //   not require the C standard library -- it's implemented by GCC.
  int jmp_ret = __builtin_setjmp (context->exit_jmp_buf);
  if (jmp_ret == 0)
    {
    stdio_init();
    ret = main (argc, argv);
    stdio_cleanup();
    }
  else
    ret = jmp_ret;


  errno = 0;

  return ret;
  }


