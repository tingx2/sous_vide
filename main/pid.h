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

  Structure Definitions/Constants

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
  float k_windup;
} pid_gain_params;

/**********************************************************************

  Functions

**********************************************************************/

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

/*
  PID initialization
*/
void pid_init();

/* PID de-initialization
*/
void pid_deinit();

#endif // PID_H