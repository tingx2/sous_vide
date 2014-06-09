/* 
  Sous-Vide

  David Dai, Tingting Zou

*/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"
#include "debug.h"
#include "timer.h"
#include "pid.h"

/*********************************************************************
  
  Main Program Loop

**********************************************************************/
int main()
{

  /* Initializations */
  debug_init();
  timer_init();


  /* Main Program Loop */
  while (1)
  {

  }

  /* De-initializations */
  timer_deinit();
  debug_deinit();

  return 0;
}
