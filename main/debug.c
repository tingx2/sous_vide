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
#include "timer.h"
#include "debug.h"

/***********************************************************************

  Locals

***********************************************************************/

#if (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_FILE)
/* Debug log */
static FILE *debug_file;
#endif // DEBUG_OUTPUT_FILE

/* Debug module list
   Names should shorter than 8 characters in length, 
   otherwise expand the array size and update padding
   in debug_fprintf and debug_printf. */
static char debug_module_names[DEBUG_MODULE_LAST+1][8] =
{
  "DEBUG",
  "PID",
  "TIMER",
  "PWM",
  "TEMP",
};

/***********************************************************************

  Functions

***********************************************************************/

/*
  void debug_printf()

  Logs to stdio a debug message.
*/
void debug_printf(uint32 module, const char * format, ...)
{
  va_list args;
  va_start (args, format);

  uint32 timestamp = timer_read32();

#if (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_FILE)
  fprintf(debug_file, "%-8s: %10u : ", debug_module_names[module], timestamp);
  vfprintf(debug_file, format, args);
  fprintf(debug_file, "\n");
#elif (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_STDIO)
  printf("%-8s: %10u : ", debug_module_names[module], timestamp);
  vprintf(format, args);
  printf("\n");
#else
  #warning "No debug_printf behavior defined.";
#endif

  va_end(args);
}

/*
  void debug_printf_notime()

  Logs to stdio a debug message.
*/
void debug_printf_notime(uint32 module, const char * format, ...)
{
  va_list args;
  va_start (args, format);

#if (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_FILE)
  fprintf(debug_file, "%-8s: %10d : ", debug_module_names[module], -1);
  vfprintf(debug_file, format, args);
  fprintf(debug_file, "\n");
#elif (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_STDIO)
  printf("%-8s: %10d : ", debug_module_names[module], -1);
  vprintf(format, args);
  printf("\n");
#else
  #warning "No debug_printf behavior defined.";
#endif

  va_end(args);
}

/*
  void debug_err_printf()

  Prints error message.
*/
void debug_err_printf(const char * format, ...)
{
  va_list args;
  va_start (args, format);

  vfprintf(stderr, format, args);

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
#if (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_FILE)
  debug_file = fopen("cook.log","w");
  
  if ( debug_file == NULL )
  {
    DEBUG_MSG_ERROR("debug_init::fopen() failed.");
  }
#endif // DEBUG_OUTPUT_FILE

  DEBUG_MSG_NOTIME(DEBUG_MODULE_DEBUG, "debug_init() complete.");
}

/*
  void debug_deinit()
  
  Closes the file handle
  for debug logging.
*/
void debug_deinit()
{
#if (DEBUG_CURRENT_OUTPUT == DEBUG_OUTPUT_FILE)
  int32 ret_val = fclose(debug_file);

  if ( ret_val != 0 )
  {
    DEBUG_MSG_ERROR("debug_deinit::fclose() failed.");
  }
#endif // DEBUG_OUTPUT_FILE

  DEBUG_MSG_NOTIME(DEBUG_MODULE_DEBUG, "debug_deinit() complete.");
}
