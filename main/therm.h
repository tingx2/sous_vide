#ifndef THERM_H
#define THERM_H

/*
	therm.h

	Header file for thermal sensor functions and related struct defines.
*/

#include <stdio.h>
#include <stdlib.h>

/**********************************************************************

  Structure Definitions

**********************************************************************/

/*
	Therm temperature state
  
	temp : temperature read
  valid : validity bit
*/

typedef struct
{
  float temp;
	bool valid;
} therm_state_t;


void therm_init();
void therm_read(therm_state_t *therm_state);
	
#endif // THERM_H