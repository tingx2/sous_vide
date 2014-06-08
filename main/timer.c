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

void init_timer();
uint64 read_timer64();
uint32 read_timer32();
void deinit_timer();

/**********************************************************************

  Functions

**********************************************************************/

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
    fprintf(stderr, "open() failed. \n");
	}

  /* Memory map the system timer */
	timer_base_addr =
	  (uint32)( mmap(NULL, TIMER_PAGE_SIZE, PROT_READ,
	                 MAP_SHARED, fd, TIMER_BASE_ADDR) );
  if ( (void*)timer_base_addr == MAP_FAILED )
	{
	  fprintf(stderr, "mmap() failed. \n");
	}

  /* Set global addresses */
  timer_32lsb_addr = timer_base_addr +
	                   TIMER_32LSB_OFFSET_ADDR;
	timer_32msb_addr = timer_base_addr +
	                   TIMER_32MSB_OFFSET_ADDR;
}

/*
  void read_timer64()

  Reads the full 64-bit Raspberry PI system
  timer - 1MHz frequency.
*/
uint64 read_timer64()
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
  void read_timer32()
  
  Read the full 32-bit Raspberry PI system
  timer - 1MHz frequency.
*/
uint32 read_timer32()
{
  return ( *((volatile uint32 *)timer_32lsb_addr) );
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
    fprintf(stderr, "munmap() failed. \n");
	}

	/* Close file */
  err = close(fd);
	if ( err == -1 )
	{
	  fprintf(stderr, "close() failed. \n");
	}
}