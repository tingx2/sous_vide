/* 
  Sous-Vide

  David Dai, Tingting Zou

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include <bcm2835.h>
#include "types.h"
#include "debug.h"
#include "timer.h"
#include "therm.h"
#include "pid.h"
#include "pwm.h"
#include "pump.h"


/**********************************************************************

  Program variables

**********************************************************************/
therm_reading_t temperature;

/*********************************************************************
  
  Main Program Loop

**********************************************************************/
int main()
{

  /* Initializations */
  debug_init();     /* This should be first. */
  timer_init();

  /* This should be before any GPIO activities. */
  uint32 ret_val = bcm2835_init(); 
  if ( ret_val == 0 )
  {
    DEBUG_MSG_ERROR("bcm2835_init() failed.");
  }

  pwm_init();
  pump_init();
  therm_init();

  pump_start();
  pwm_set_duty(25);

  /* Main Program Loop */
  while (1)
  {
    pwm_run();
    therm_capture();    
    therm_read(&temperature);
  }

  pump_stop();

  /* De-initializations */
  pump_deinit();
  pwm_deinit();

  /* This should be after all GPIO activities. */
  ret_val = bcm2835_close();
  if ( ret_val == 0 )
  {
    DEBUG_MSG_ERROR("bcm2835_close() failed.");
  }
  timer_deinit();
  debug_deinit();   /* This should be last. */

  return 0;
}
