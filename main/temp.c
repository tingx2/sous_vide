#include <stdio.h>
#include <stdlib.h>
#include "temp.h"

static FILE *file_p;
static char *line;

void temp_init()
{
  file_p = fopen("/sys/bus/w1/devices/28-0014022cb8ff/w1_slave", "r");
	line = malloc(40*sizeof(char));
	if (file_p == NULL)
	{
		printf("temp_init() failed\n");
		//add error catching
	}
}


int temp_read()
{
	size_t len = 0;	
	ssize_t read = 0;	
	int i= 0;
	rewind(file_p);
	while((read = getline(&line, &len, file_p)) > 0);
	return atoi(&line[29]);
}
