#include <stdio.h>
#include <stdlib.h>

static FILE *file_p;

void temp_init()
{
  file_p = fopen("/sys/bus/w1/devices/28-0014022cb8ff/w1_slave", "r");
	if (file_p == NULL)
	{
		printf("Can't read sensor!\n");
		return -1;
	}
	int temp = sensor_read();
	printf("temp:%d\n", temp);
	return 0;
}

int temp_read()
{
	size_t len = 0;	
	ssize_t read = 0;	
	int i= 0;
	char * line = NULL;
	rewind(file_p);
	while((read = getline(&line, &len, file_p)) > 0)
	{
		if (i==1)
		{
			char* token = strtok(line, " ");
			char* lastline=malloc(20*sizeof(char));
			while (token) 
			{
		    strcpy(lastline, token);
			  token = strtok(NULL, " ");
			}
			lastline = lastline+2;
			return atoi(lastline);
		}
		i++;
		free(line);
		line = NULL;
	}
}
	
