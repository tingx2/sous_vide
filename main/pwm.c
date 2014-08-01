/* 
  pwm.c

  PWM related routines.  Toggles the relay-switch connected to
  Raspberry PI GPIO for PWM controlling the heating element

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"
#include "debug.h"
#include "timer.h"
#include "pwm.h"
#include "gpio.h"

/**********************************************************************

  Definitions and Variables

**********************************************************************/

/* PWM frequency and periods */
#define PWM_FREQ_HZ         10
#define PWM_PERIOD_TICKS    (TIMER_FREQ_HZ/PWM_FREQ_HZ)
#define PWM_TIME_UNIT       (PWM_PERIOD_TICKS/PWM_MAX_DUTY_CYCLE)

/* Start delay for the PWM module */
#define PWM_DELAY_1MS       (TIMER_FREQ_HZ/1000)
#define PWM_START_DELAY     (100 * PWM_DELAY_1MS)

/* PWM GPIO operations */
#define PWM_GPIO_SET()  GPIO_SET(GPIO_PWM);
#define PWM_GPIO_CLR()  GPIO_CLR(GPIO_PWM);

/* PWM pin states */
typedef enum
{
  PWM_PIN_LOW = 0,
  PWM_PIN_HIGH,
} pwm_pin_state_e;

/* Flag to indicate if PWM is running */
static boolean pwm_is_running;

/* Flag to indicate current pin state */
static pwm_pin_state_e pwm_pin_state;

/* Current duty cycle */
static uint32 pwm_current_duty;

/* Action time increments based on duty cycle */
static uint32 pwm_time_duration_high;
static uint32 pwm_time_duration_low;

/* Next action times for toggling  high/low */
static uint32 pwm_next_action_time;

/**********************************************************************

  Functions

**********************************************************************/

/*
  void pwm_run()

  Main PWM function to handle toggling.
*/
void pwm_run()
{
  if (!pwm_is_running)
    return;

  uint32 curr_time;
  boolean toggle = timer_is_elapsed32(pwm_next_action_time, &curr_time);

  if (!toggle)
    return;

  if ( pwm_current_duty == PWM_MAX_DUTY_CYCLE )
  {
    pwm_next_action_time += pwm_time_duration_high;
    pwm_pin_state = PWM_PIN_HIGH;
    PWM_GPIO_SET();
    return;
  }

  if ( pwm_current_duty == PWM_MIN_DUTY_CYCLE )
  {
    pwm_next_action_time += pwm_time_duration_low;
    pwm_pin_state = PWM_PIN_LOW;
    PWM_GPIO_CLR();
    return;
  }

  if ( pwm_pin_state == PWM_PIN_LOW )
  {
    pwm_next_action_time += pwm_time_duration_high;
    pwm_pin_state = PWM_PIN_HIGH;
    PWM_GPIO_SET();
  }
  else
  { 
    pwm_next_action_time += pwm_time_duration_low;
    pwm_pin_state = PWM_PIN_LOW;
    PWM_GPIO_CLR();
  }

  DEBUG_MSG_HIGH(DEBUG_MODULE_PWM, "Toggle heating pin to %d", (uint32)pwm_pin_state);
}

/*
  void pwm_stop()

  Stops the PWM routine.
*/
void pwm_stop()
{
  pwm_is_running = FALSE;
}

/*
  void pwm_start()

  Starts the PWM routine.
*/
void pwm_start()
{
  pwm_is_running = TRUE;
}

/*
  void pwm_set_duty()

  Sets the action time increments
  based on the duty cycle.

  Duty cycle range is [0 - 100]
*/
void pwm_set_duty(uint32 duty)
{
  pwm_time_duration_high = PWM_TIME_UNIT * duty;
  pwm_time_duration_low  = PWM_TIME_UNIT * (PWM_MAX_DUTY_CYCLE - duty);
  pwm_current_duty       = duty;

  DEBUG_MSG_MID(DEBUG_MODULE_PWM, "Duty cycle update, duty: %d%%, high_duration: %d(ms), low_duration: %d(ms)", 
                duty, pwm_time_duration_high, pwm_time_duration_low);
}

/*
  void pwm_init()

  Initializes frequency of the PWM.
*/
void pwm_init()
{
  GPIO_DIRECTION_OUTPUT(GPIO_PWM);

  pwm_pin_state           = PWM_PIN_LOW;
  pwm_current_duty        = PWM_MIN_DUTY_CYCLE;
  pwm_time_duration_high  = 0;
  pwm_time_duration_low   = PWM_TIME_UNIT;
  pwm_next_action_time    = timer_read32() + PWM_START_DELAY;

  pwm_start();

  DEBUG_MSG_NOTIME(DEBUG_MODULE_PWM, "pwm_init() complete.");
}

/*
  void pwm_init()

  Cleans up PWM module.
*/
void pwm_deinit()
{
  pwm_stop();

  DEBUG_MSG_NOTIME(DEBUG_MODULE_PWM, "pwm_deinit() complete.");
}