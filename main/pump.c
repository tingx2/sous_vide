/* 
  pump.c

  Water pump driver.  Toggles on the GPIO for the water pump
  to circulate the water inside the tank.

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"
#include "debug.h"
#include "gpio.h"
#include "pump.h"

/**********************************************************************

  Definitions and Variables

**********************************************************************/

/**********************************************************************

  Functions

**********************************************************************/

/*
  void pump_stop()

  Stops the water pump.
*/
void pump_stop()
{
  GPIO_CLR(GPIO_PUMP);

  DEBUG_MSG_HIGH(DEBUG_MODULE_PUMP, "Stop water pump.");
}

/*
  void pump_start()

  Starts the water pump.
*/
void pump_start()
{
  GPIO_SET(GPIO_PUMP);

  DEBUG_MSG_HIGH(DEBUG_MODULE_PUMP, "Start water pump.");
}

/*
  void pump_init()

  Initializes the driver for the water pump.
*/
void pump_init()
{
  GPIO_DIRECTION_OUTPUT(GPIO_PUMP);

  DEBUG_MSG_NOTIME(DEBUG_MODULE_PWM, "pump_init() complete.");
}

/*
  void pump_deinit()

  Cleans up water pump driver.
*/
void pump_deinit()
{
  pump_stop();

  DEBUG_MSG_NOTIME(DEBUG_MODULE_PWM, "pump_deinit() complete.");
}