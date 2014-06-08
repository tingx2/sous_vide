/*
  debug.c

	Source file for debug routines.

*/

/***********************************************************************

  Header Files

***********************************************************************/

#include "stdio.h"

static FILE *debug_file;

/***********************************************************************

  Functions

***********************************************************************/

/*
  void debug_init()

	Opens the file handle for
	logging out debug info to
	text file.
*/
void debug_init()
{
  debug_file = fopen("debug.txt","w");
	
  if ( debug_file == NULL )
	{
	  fprintf(stderr, "fopen() failed.\n");
	}
}

/*
  void debug_deinit()
	
	Closes the file handle
	for debug logging.
*/
void debug_deinit()
{
  fclose(debug_file);
}