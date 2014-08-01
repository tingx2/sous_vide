#ifndef PUMP_H
#define PUMP_H

/* 
  pump.h

  Header file for Water Pump related routines.

*/

/**********************************************************************

  Header Files

**********************************************************************/
#include "types.h"

/**********************************************************************

  Definitions, Variables

**********************************************************************/


/**********************************************************************

  Functions

**********************************************************************/

/*
  void pump_init()

  Initializes the water pump driver.
*/
void pump_init();

/*
  void pump_deinit()

  Cleans up water pump driver.
*/
void pump_deinit();

/*
  void pump_start()

  Starts the water pump.
*/
void pump_start();

/*
  void pump_stop()

  Stops the water pump.
*/
void pump_stop();

#endif // PUMP_H