/* 
  pid.c

  Source file for PID related routines.

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "debug.h"
#include "timer.h"
#include "pid.h"
#include "pwm.h"
#include "therm.h"

/**********************************************************************

  Structure Definitions

**********************************************************************/

#define PID_LOOP_WAIT_IN_NS 750000 /* Wait 750us before updating PID */

/*
  Control Loop
  Error Parameters

    err   : Current Error
    err_i : Integral Error
    err_d : Derivative Error
*/
typedef struct
{
  float err;
  float err_i;
  float err_d;
} pid_error_params;

/*
  Main Global Structure

    Contains functional parameters
    used in the contro loop.
*/
typedef struct 
{
  pid_gain_params gain;             /* Loop parameters */   
  pid_error_params error;           /* Error pameters */
  float temp_setpoint;              /* Desired temperature (in Celsius) */
  therm_reading_t temp;             /* Current Temperature */
} pid_main_params;

/* PID state */
 typedef enum
 {
   PID_STOPPED,
   PID_RUNNING,
 } pid_state_e;

/**********************************************************************

  Structure Declarations

**********************************************************************/

/* Main Loop Parameters */
pid_main_params pid_params;

 /* PID loop state */
static pid_state_e pid_state;

/* The system timestamp when the last time PID update occurred. */
static uint32 pid_last_timestamp;

/**********************************************************************

  Functions

**********************************************************************/

/*
  void pid_update_temp_setpoint()

  Updates the set temperature
  to converge to in the PID loop.
*/
void pid_update_temp_setpoint(float temp)
{
  pid_params.temp_setpoint = temp;
}

/*
  void pid_ZN_gain()

  Set the gain parameters using Ziegel-Nichols
  step_response algorithm.
*/
#if 0
void pid_ZN_gain(ZN_params_t *ZN_params)
{
  pid_params.gain = 1.2 / ZN_;
  pid_params.gain.k_p = 3.4 * ZN_params.L;
  pid_params.gain.k = 0.5 * ZN_params.L;
  pid_gain.k_i = 2 *;
}
#endif

/*
  void pid_set_gain()

  Updates the gain loop parameters.
*/
void pid_set_gain(pid_gain_params *gain)
{
  pid_params.gain = *gain;
}

/*
  void pid_loop()

    Calculates the updated PID
    output and parameters for each
    iteration.
*/
void pid_loop()
{
  if (pid_state != PID_RUNNING)
    return;

  uint32 dummy_time;
  boolean is_update_pid =
    timer_is_elapsed32((pid_last_timestamp + PID_LOOP_WAIT_IN_NS), &dummy_time);
  if (!is_update_pid)
  {
    DEBUG_MSG_HIGH(DEBUG_MODULE_PID, "Timer not expired. Time: %d", dummy_time);
    return;
  }

  /* Update the last time PID was read for scheduling next iteration. */
  pid_last_timestamp = timer_read32();

  /* Read updated temperatures. If last reading was invalid,
     skip this PID loop iteration. */
  therm_reading_t *temp = &(pid_params.temp);
  therm_read(temp);
  if (!temp->is_valid)
  {
    DEBUG_MSG_HIGH(DEBUG_MODULE_PID, "Temperature read failed");
    return;
  }

  /* Do PID calculation. */
  float t_current = pid_params.temp.temperature;
  float t_desired = pid_params.temp_setpoint;
  pid_gain_params *gain_p = &(pid_params.gain);
  pid_error_params *error_p = &(pid_params.error);

  float err, err_i, err_d;
  err   = t_desired - t_current;
  err_i = err + error_p->err_i;  
  err_d = err - error_p->err;

  float p, i, d;
  p = gain_p->k_p * err;
  i = gain_p->k_i * err_i;  
  d = gain_p->k_d * err_d;

  /* Scale/saturate the PID output and set the duty cycle. */
  float pid_raw = p + i + d;

  /* Add 0.5 to do rounding with truncation. */
  float pid_scaled = pid_raw + 0.5;
  pid_scaled = MAX(pid_raw, 0);
  pid_scaled = MIN(pid_scaled, 100);


  pwm_set_duty((uint32)pid_scaled);

  /* Calculate saturation error for anti-windup loop. */
  float err_saturate = (pid_scaled - pid_raw) * (gain_p->k_windup);

  /* Update error for next iteration. */
  error_p->err = err;
  error_p->err_d = err_d;
  error_p->err_i = err_i + err_saturate;

  DEBUG_MSG_MED(DEBUG_MODULE_PID, "err: %5.2f, err_d: %5.2f, err_i: %5.2f, err_sat: %5.2f",
                 error_p->err, error_p->err_d, error_p->err_i, err_saturate);
  DEBUG_MSG_MED(DEBUG_MODULE_PID, "pid_raw: %5.2f, pid_scaled: %5.2f",
                 pid_raw, pid_scaled);
}

/*
  PID initialization
*/
void pid_init()
{
  pid_state = PID_RUNNING;

  memset(&pid_params, 0x0, sizeof(pid_params));

  /* Update the last time PID was read for scheduling next iteration. */
  pid_last_timestamp = timer_read32();

}

/* PID de-initialization
*/
void pid_deinit()
{
  pid_state = PID_STOPPED;
}
