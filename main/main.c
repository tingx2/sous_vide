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
  pid_init();

  pump_start();
  pid_update_temp_setpoint(50);
  pid_gain_params pid_gain;
  pid_gain.k_p = 1;
  pid_gain.k_d = 1;
  pid_gain.k_i = 0;
  pid_set_gain(&pid_gain);

  /* Main Program Loop */
  while (1)
  {
    pwm_run();
    therm_capture();
    pid_loop();
  }

  pump_stop();

  /* De-initializations */
  pump_deinit();
  pid_deinit();
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
