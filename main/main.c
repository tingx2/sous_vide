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
    
    /* Test reading 64-bit timer */
    uint64 t1 = read_timer64();
    debug_printf(DEBUG_MODULE_DEBUG, "read_timer64 0x%x", t1);

  }

  /* De-initializations */
  timer_deinit();
  debug_deinit();

  return 0;
}
