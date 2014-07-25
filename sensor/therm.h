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


void temp_init();
int temp_read();
	
