/**********************************************************************

  Timer Driver

**********************************************************************/

/**********************************************************************

  Header Files

**********************************************************************/

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "timer.h"
#include "debug.h"

/**********************************************************************

  Definitions

**********************************************************************/

#define TIMER_PAGE_SIZE            (4096)
#define TIMER_BASE_ADDR            (0x20003000)
#define TIMER_32LSB_OFFSET_ADDR    (0x4)
#define TIMER_32MSB_OFFSET_ADDR    (0x8)

/**********************************************************************

  Global Variables

**********************************************************************/

static int32 fd;
static uint32 timer_base_addr;
static uint32 timer_32lsb_addr;
static uint32 timer_32msb_addr;

/**********************************************************************

  Function Prototypes

**********************************************************************/

/**********************************************************************

  Functions

**********************************************************************/

/*
  boolean timer_is_elapsed32( uint32 *time )

    Checks whether or not your scheduled action time has elapsed.
    Returns the time read out as part of the check.
*/
boolean timer_is_elapsed32 ( uint32 action_time, uint32 *time_readback )
{
  boolean time_expired = FALSE;

  uint32 time = timer_read32();
  *time_readback = time;

  if ( (uint32)(time - action_time) < (uint32)MAX_INT/2 )
  {
    time_expired = TRUE;
  }

  return time_expired;
}

/*
  boolean timer_is_elapsed64( uint64 *time )

    Checks whether or not your scheduled action time has elapsed.
    Returns the time read out as part of the check.
*/
boolean timer_is_elapsed64 ( uint64 action_time, uint64 *time_readback )
{
  boolean time_expired = FALSE;

  uint64 time = timer_read64();
  *time_readback = time;

  if ( (uint64)(time - action_time) < (uint64)MAX_LONGINT/2 )
  {
    time_expired = TRUE;
  }

  return time_expired;
}

/*
  void timer_read64()

  Reads the full 64-bit Raspberry PI system
  timer - 1MHz frequency.
*/
uint64 timer_read64()
{
  uint32 lsb, msb, msb_2;
  do
  {
    msb   = *((volatile uint32 *)timer_32msb_addr);
    lsb   = *((volatile uint32 *)timer_32lsb_addr);
    msb_2 = *((volatile uint32 *)timer_32msb_addr);
  } while (msb != msb_2);
  return ( (uint64)msb << 32 | lsb );
}

/* 
  void timer_read32()
  
  Read the full 32-bit Raspberry PI system
  timer - 1MHz frequency.
*/
uint32 timer_read32()
{
  return ( *((volatile uint32 *)timer_32lsb_addr) );
}

/*
  void timer_init()

  Initializes the timer handlers.
*/
void timer_init()
{
  /* Open file to memory */     
  fd = open("/dev/mem", O_RDONLY);
  if ( fd == -1 )
  {
    debug_err_printf("timer_init::open() failed.");
  }

  /* Memory map the system timer */
  timer_base_addr =
    (uint32)( mmap(NULL, TIMER_PAGE_SIZE, PROT_READ,
                   MAP_SHARED, fd, TIMER_BASE_ADDR) );
  if ( (void*)timer_base_addr == MAP_FAILED )
  {
    debug_err_printf("timer_init::mmap() failed.");
  }

  /* Set global addresses */
  timer_32lsb_addr = timer_base_addr +
                     TIMER_32LSB_OFFSET_ADDR;
  timer_32msb_addr = timer_base_addr +
                     TIMER_32MSB_OFFSET_ADDR;

  debug_fprintf(DEBUG_MODULE_TIMER, "timer_init() complete.");
}

/* 
  void timer_deinit()

  Cleanup deinit handlers.
*/
void timer_deinit()
{
  /* Unmap system timer memory */
  int32 err = munmap((void*)timer_base_addr, TIMER_PAGE_SIZE);
  if ( err == -1 )
  {
    debug_err_printf("time_deinit::munmap() failed.");
  }

  /* Close file */
  err = close(fd);
  if ( err == -1 )
  {
    debug_err_printf("timer_deinit::close() failed.");
  }

  debug_fprintf(DEBUG_MODULE_TIMER, "timer_deinit() complete.");
}