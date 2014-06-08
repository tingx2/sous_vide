#ifndef PID_H
#define PID_H

/* 
  pid.h

	Header file for PID related routines.

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"

/**********************************************************************

  Structure Definitions

**********************************************************************/

/*
  Control Loop
	Gain Parameters

  k_p : Proportional Gain
	k_i : Integral Gain
	k_d : Derivative Gain
*/
typedef struct
{
  float k_p;
	float k_i;
	float k_d;
} pid_gain_params;

/**********************************************************************

  Functions

**********************************************************************/

/*
  Returns the most recent,
	unscaled PID output.
*/
float pid_get_pid_output_raw();

/* 
  Updates the set temperature
  to converge to in the PID loop.
*/
void pid_update_temp_setpoint(float temp);

/*
  Sets new gain parameters.
*/
void pid_set_gain(pid_gain_params *params);

/*
  Main PID control loop
*/
void pid_loop();

#endif // PID_H