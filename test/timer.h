#ifndef TIMER_H
#define TIMER_H

/**********************************************************************

  Timer Header File

**********************************************************************/

/**********************************************************************

  Header Files

**********************************************************************/

#include "types.h"

/*********************************************************************

  Function Prototypes

*********************************************************************/

/*
  void init_timer()

	Initializes the 1MHz system
	timer module.
	
	Maps the physical timer to
	a virtual address to read.
*/
void init_timer();

/*
  uint64 read_timer64()

	Reads out the full 64-bit
	1MHz system timer.
*/
uint64 read_timer64();

/*
  uint32 read_timer32()
	
	Reads out the full 32-bit
	1MHz system timer.
*/
uint32 read_timer32();

/*
  void deinit_timer()

  De-initializes the 1MHz system
	timer module.
*/
void deinit_timer();

#endif // TIMER_H
