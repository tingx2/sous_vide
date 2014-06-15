#ifndef TEMP_H
#define PWM_H

/*
  temp.h

	Header file for temperature sensor routines.
	
*/

/**********************************************************************

	Header Files

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>

/**********************************************************************

	Functions

**********************************************************************/

/*
  void temp_init()

	Initializes temperature drivers and file pointer to sensor.
*/
void temp_init();

/*
  int temp_read()

	Read temp from sensor file pointer and return raw value as int
*/
int temp_read();

/*
  void temp_deinit()

	clean up for temperature module	
*/
void temp_deinit();

	
