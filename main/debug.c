/*
  debug.c

  Source file for debug routines.

*/

/***********************************************************************

  Header Files

***********************************************************************/

#include "types.h"
#include "stdio.h"
#include "stdarg.h"
#include "debug.h"

/***********************************************************************

  Locals

***********************************************************************/

/* Debug log */
static FILE *debug_file;

/* Debug module list */
static char debug_module_names[DEBUG_MODULE_LAST][12] =
{
  "DEBUG",
  "PID"
  "TIMER",
  "PWM",
};

/***********************************************************************

  Functions

***********************************************************************/

/*
  void debug_printf_int()

  Logs to file a debug message.
*/
void debug_fprintf(uint32 module, const char * format, ...)
{
  va_list args;
  va_start (args, format);
 
  fprintf(debug_file, "%-12s: ", debug_module_names[module]);
  vfprintf(debug_file, format, args);
  fprintf(debug_file, "\n");

  va_end(args);
}

/*
  void debug_printf_int()

  Logs to stdio a debug message.
*/
void debug_printf(uint32 module, const char * format, ...)
{
  va_list args;
  va_start (args, format);

  printf("%-12s: ", debug_module_names[module]);
  vprintf(format, args);
  printf("\n");

  va_end(args);
}

/*
  void debug_init()

  Opens the file handle for
  logging out debug info to
  text file.
*/
void debug_init()
{
  debug_file = fopen("debug.txt","w");
  
  if ( debug_file == NULL )
  {
    fprintf(stderr, "fopen() failed.\n");
  }
}

/*
  void debug_deinit()
  
  Closes the file handle
  for debug logging.
*/
void debug_deinit()
{
  fclose(debug_file);
}