/* 
  Sous-Vide

  David Dai, Tingting Zou

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"
#include "debug.h"
#include "timer.h"
#include "temp.h"
#include "pid.h"
#include "pwm.h"

// XXX temp for debug
static uint32 loop_count = 0;

/*********************************************************************
  
  Main Program Loop

**********************************************************************/
int main()
{

  /* Initializations */
  debug_init();     /* This should be first. */
  timer_init();
  temp_init();
  pwm_init();
  pwm_set_duty(25);

  /* Main Program Loop */
  while (1)
  {

    pwm_run();
		int test = temp_read();
    if (loop_count++ > 10000000)  /* XXX Run for 10 seconds */
      break;

  }

  /* De-initializations */
  pwm_deinit();
  temp_deinit();
  timer_deinit();
  debug_deinit();   /* This should be last. */

  return 0;
}
