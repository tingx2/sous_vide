/* 
  pid.c

    Source file for PID related routines.

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "debug.h"
#include "pid.h"

/**********************************************************************

  Structure Definitions

**********************************************************************/

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
  float temp_current;               /* Current Temperature */
  float pid_output_raw;             /* PID output */
} pid_main_params;

/**********************************************************************

  Structure Declarations

**********************************************************************

/* Main Loop Parameters */
pid_main_params pid_params;

/**********************************************************************

  Functions

**********************************************************************/

pid_gain_params *pid_get_gain();
pid_error_params *pid_get_error();
float pid_get_temp_setpoint();
float pid_get_temp_current();

void pid_update_temp_current(float temp);
void pid_update_gain(pid_gain_params *params);
void pid_update_output_raw(float pid_output);

/*
  pid_gain_params *pid_get_gain()

    Returns a pointer to the gain
    parameters of the control loop.
*/
pid_gain_params *pid_get_gain()
{
  return (&(pid_params.gain));
}

/*
  pid_error_params *get_error()

    Returns a pointer to the error
    parameters of the control loop.
*/
pid_error_params *pid_get_error()
{
  return (&(pid_params.error));
}

/*
  float pid_get_temp_setpoint()

    Gets the set temperature.
*/
float pid_get_temp_setpoint()
{
  return (pid_params.temp_setpoint); 
}

/* 
  float pid_get_temp_current();

    Gets the current temperature
    read out by the sensor.
*/
float pid_get_temp_current()
{
  return (pid_params.temp_current);
}

/*
  float pid_get_pid_output_raw()

    Gets the latest PID
    output calculated.
*/
float pid_get_pid_output_raw()
{
  return (pid_params.pid_output_raw);
}

/*
  float pid_update_temp_current

    Updates the current temperature
    in the control loop.
*/
void pid_update_temp_current(float temp)
{
  pid_params.temp_current = temp;
}

/*
  float pid_update_temp_setpoint

    Updates the desired temperature
    to be reached by the control loop.
*/
void pid_update_temp_setpoint(float temp)
{
  pid_params.temp_setpoint = temp;
}

/*
  float pid_update_output_raw

    Updates the PID output value.
*/
void pid_update_output_raw(float pid_output)
{
  pid_params.pid_output_raw = pid_output;
}

/*
  void pid_loop()

    Calculates the updated PID
    output and parameters for each
    iteration.
*/
void pid_loop()
{
  pid_gain_params *gain_p =
      pid_get_gain();

  pid_error_params *error_p =
      pid_get_error();

    float t_current =
      pid_get_temp_current();

  float t_desired =
      pid_get_temp_setpoint();

  /* Core PID Loop */
  error_p->err   = t_current - t_desired;
    error_p->err_i = error_p->err_i + error_p->err;
    error_p->err_d = error_p->err   - error_p->err_d;

    float p, i, d;
    p = gain_p->k_p * error_p->err;
    i = gain_p->k_i * error_p->err_i;
    d = gain_p->k_d * error_p->err_d;

  pid_update_output_raw( p+i+d );
}