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
  void timer_init()

  Initializes the 1MHz system
  timer module.
	
  Maps the physical timer to
  a virtual address to read.
*/
void timer_init();

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
  void timer_deinit()

  De-initializes the 1MHz system
  timer module.
*/
void timer_deinit();

#endif // TIMER_H