/* 
  pwm.c

  PWM related routines.  Toggles the relay-switch connected to
  Raspberry PI GPIO for PWM controlling the heating element

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include <bcm2835.h>
#include "types.h"
#include "debug.h"
#include "timer.h"
#include "pwm.h"

/**********************************************************************

  Definitions and Variables

**********************************************************************/

#define PWM_GPIO_PIN        RPI_GPIO_P1_11

/* PWM frequency and periods */
#define PWM_FREQ_HZ         10
#define PWM_PERIOD_TICKS    (TIMER_FREQ_HZ/PWM_FREQ_HZ)
#define PWM_TIME_UNIT       (PWM_PERIOD_TICKS/PWM_MAX_DUTY_CYCLE)

/* Start delay for the PWM module */
#define PWM_DELAY_1MS       (TIMER_FREQ_HZ/1000)
#define PWM_START_DELAY     (100 * PWM_DELAY_1MS)

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
    bcm2835_gpio_set(PWM_GPIO_PIN);
    return;
  }

  if ( pwm_current_duty == PWM_MIN_DUTY_CYCLE )
  {
    pwm_next_action_time += pwm_time_duration_low;
    pwm_pin_state = PWM_PIN_LOW;
    bcm2835_gpio_clr(PWM_GPIO_PIN);
    return;
  }

  if ( pwm_pin_state == PWM_PIN_LOW )
  {
    pwm_next_action_time += pwm_time_duration_high;
    pwm_pin_state = PWM_PIN_HIGH;
    bcm2835_gpio_set(PWM_GPIO_PIN);
  }
  else
  { 
    pwm_next_action_time += pwm_time_duration_low;
    pwm_pin_state = PWM_PIN_LOW;
    bcm2835_gpio_clr(PWM_GPIO_PIN);
  }

  debug_fprintf(DEBUG_MODULE_PWM, "Toggle @ time: 0x%x to %d", curr_time, (uint32)pwm_pin_state);
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
  void pwn_set_duty()

  Sets the action time increments
  based on the duty cycle.

  Duty cycle range is [0 - 100]
*/
void pwn_set_duty(uint32 duty)
{
  pwm_time_duration_high = PWM_TIME_UNIT * duty;
  pwm_time_duration_low  = PWM_TIME_UNIT * (PWM_MAX_DUTY_CYCLE - duty);
  pwm_current_duty       = duty;

  debug_fprintf(DEBUG_MODULE_PWM, "Duty cycle update, duty: %d, high_duration: %d, low_duration: %d", 
                duty, pwm_time_duration_high, pwm_time_duration_low);
}

/*
  void pwm_init()

  Initializes frequency of the PWM.
*/
void pwm_init()
{
  uint32 ret_val = bcm2835_init();
  if ( ret_val == 0 )
  {
    debug_err_printf("pwm_init::bcm2835_init() failed.");
  }

  bcm2835_gpio_fsel(PWM_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);

  pwm_pin_state           = PWM_PIN_LOW;
  pwm_current_duty        = PWM_MIN_DUTY_CYCLE;
  pwm_time_duration_high  = 0;
  pwm_time_duration_low   = PWM_TIME_UNIT;
  pwm_next_action_time    = timer_read32() + PWM_START_DELAY;

  pwm_start();

  debug_fprintf(DEBUG_MODULE_PWM, "pwm_init() complete.");
}

/*
  void pwm_init()

  Cleans up PWM module.
*/
void pwm_deinit()
{
  pwm_stop();

  uint32 ret_val = bcm2835_close();
  if ( ret_val == 0 )
  {
    debug_err_printf("pwm_deinit::bcm2835_close() failed.");
  }

  debug_fprintf(DEBUG_MODULE_PWM, "pwm_deinit() complete.");
}