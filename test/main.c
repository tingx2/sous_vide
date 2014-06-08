/**********************************************************************

  Main

***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "types.h"
#include "timer.h"

int main()
{
  init_timer();

  while(1)
	{
	  uint64 t1 = read_timer64();

    usleep(1);

		uint64 t2 = read_timer64();

		printf("t1:%lld, t2:%lld, delta: %lld\n",
		       t1, t2, t2-t1);
	}

	deinit_timer();

  return 0;
}
