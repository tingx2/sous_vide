#ifndef DEBUG_H
#define DEBUG_H

/* 
  debug.h

  Header file for debug routines.
*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"

/**********************************************************************

  Definitions

**********************************************************************/

/* Set the debug options using these flag */
#define DEBUG_CURRENT_VERBOSITY   DEBUG_VERBOSE_HIGH
#define DEBUG_CURRENT_OUTPUT      DEBUG_OUTPUT_STDIO

/* Selects either file or terminal as the output location
   for debug messages */
#define DEBUG_OUTPUT_STDIO      0
#define DEBUG_OUTPUT_FILE       1

/* Sets the level of debug messages */
#define DEBUG_VERBOSE_NONE      0
#define DEBUG_VERBOSE_LOW       1
#define DEBUG_VERBOSE_MID       2
#define DEBUG_VERBOSE_HIGH      3

/* Debug message macros */
#if (DEBUG_CURRENT_VERBOSITY >= DEBUG_VERBOSE_LOW)
  #define DEBUG_MSG_LOW(module, ...)    \
    debug_printf(module, __VA_ARGS__)
#else           
  #define DEBUG_MSG_LOW(module, ...)
#endif

#if (DEBUG_CURRENT_VERBOSITY >= DEBUG_VERBOSE_MID)
  #define DEBUG_MSG_MID(module, ...)    \
    debug_printf(module, __VA_ARGS__)
#else              
  #define DEBUG_MSG_MID(module, ...)
#endif

#if (DEBUG_CURRENT_VERBOSITY >= DEBUG_VERBOSE_HIGH)
  #define DEBUG_MSG_HIGH(module, ...)   \
    debug_printf(module, __VA_ARGS__)
#else         
  #define DEBUG_MSG_HIGH(module, ...)
#endif

#define DEBUG_MSG_ERROR(...) debug_err_printf(__VA_ARGS__)

#define DEBUG_MSG_NOTIME(module, ...) debug_printf_notime(module, __VA_ARGS__)

/* Debug module identifiers */
#define DEBUG_MODULE_DEBUG    0x0
#define DEBUG_MODULE_PID      0x1
#define DEBUG_MODULE_TIMER    0x2
#define DEBUG_MODULE_PWM      0x3
#define DEBUG_MODULE_THERM    0x4
#define DEBUG_MODULE_LAST     (DEBUG_MODULE_THERM)

/**********************************************************************

  Function Prototypes

**********************************************************************/

/*
  void debug_init()

  Opens the file handle for
  logging out debug info to
  text file.
*/
void debug_init();

/*
  void debug_deinit()
  
  Closes the file handle
  for debug logging.
*/
void debug_deinit();

/*
  void debug_printf()

  Logs to stdio a debug message.
*/
void debug_printf(uint32 module, const char * format, ...);

/*
  void debug_printf_notime()

  Logs to stdio a debug message.
  No timestamp readout.  For sure when timer
  module has not been initialized yet.
*/
void debug_printf_notime(uint32 module, const char * format, ...);

/*
  void debug_err_printf()

  Prints error message.
*/
void debug_err_printf(const char * format, ...);

#endif // DEBUG_H
