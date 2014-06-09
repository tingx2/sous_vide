#ifndef PWM_H
#define PWM_H

/* 
  pwm.h

  Header file for PWM related routines.

*/

/**********************************************************************

  Header Files

**********************************************************************/
#include "types.h"

/**********************************************************************

  Definitions, Variables

**********************************************************************/
#define PWM_MIN_DUTY_CYCLE      0
#define PWM_MAX_DUTY_CYCLE      100

/**********************************************************************

  Functions

**********************************************************************/

/*
  void pwm_init()

  Initializes frequency of the PWM.
*/
void pwm_init();

/*
  void pwm_init()

  Cleans up PWM module.
*/
void pwm_deinit();

/*
  void pwn_set_duty()

  Sets the action time increments
  based on the duty cycle.

  Duty cycle range is [0 - 100]
*/
void pwn_set_duty(uint32 duty);

/*
  void pwm_run()

  Main PWM function to handle toggling.
  This should be called after pwm_init() is called
  and pwm_set_duty() is called.
*/
void pwm_run();

/*
  void pwm_start()

  Starts the PWM routine.
*/
void pwm_start();

/*
  void pwm_stop()

  Stops the PWM routine.
*/
void pwm_stop();

#endif // PWN_H