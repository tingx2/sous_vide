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

#define DEBUG_MODULE_DEBUG		0x0
#define DEBUG_MODULE_PID		0x1
#define DEBUG_MODULE_TIMER		0x2
#define DEBUG_MODULE_PWM		0x3
#define DEBUG_MODULE_LAST		(DEBUG_MODULE_PWM)

/**********************************************************************

  Function Prototypes

**********************************************************************/

void debug_init();
void debug_deinit();

void debug_fprintf(uint32 module, const char * format, ...);
void debug_printf(uint32 module, const char * format, ...);

#endif // DEBUG_H