/* 
  temp.c

  Temperature sensor driver initialization and reads
*/


#include <stdio.h>
#include <stdlib.h>
#include "temp.h"
#include "debug.h"

static FILE *sensor_fp;
static char *line;

/*
	void temp_init()
	Initializes temperature driver and opens file pointer to device.
*/

void temp_init()
{

	//System calls to initialize sensor driver.
	system("sudo modprobe w1-gpio");
	system("sudo modprobe w1-therm");
  
	//Opens file pointer to sensor device.
	sensor_fp = fopen("/sys/bus/w1/devices/28-0014022cb8ff/w1_slave", "r");
	
	line = malloc(40*sizeof(char));

  if (sensor_fp == NULL)
	{
		DEBUG_MSG_ERROR("temp_init::fopen() failed.");
	}
	
	DEBUG_MSG_NOTIME(DEBUG_MODULE_TEMP, "temp_init() complete.");
}

/*
	void temp_deinit()
	Free line buffer and file pointer
*/

void temp_deinit()
{

	//close file pointer to sensor device.
	fclose(sensor_fp);

	//free line buffer.
	free(line);

	DEBUG_MSG_NOTIME(DEBUG_MODULE_TEMP, "temp_deinit() complete.");
}


/*
	void temp_read()
	reads sensor and outputs raw digital temperature read as XX.XXX Celcius
*/

int temp_read()
{
	size_t len = 0;	
	
	//return file pointer for sensor to beginning for re-read
	rewind(sensor_fp);
	
	//read until last line
	while((getline(&line, &len, sensor_fp)) > 0);
	
	DEBUG_MSG_HIGH(DEBUG_MODULE_TEMP, "Temp: %s", &line[29]);
	
	//return read value as int
	return atoi(&line[29]);
}
