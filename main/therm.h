#ifndef THERM_H
#define THERM_H

/*
  therm.h

  Header file for the DS18B20 thermometer driver.
*/

#include "types.h"

/**********************************************************************

  Structure Definitions

**********************************************************************/

/*
  Thermometer temperature state
  
  valid       : TRUE  = temperature read successful
                FALSE = temperature read unsuccessful
  temperature : temperature in celsius
  timestampt  : The system timestamp when the reading was
                triggered
*/
typedef struct
{
  boolean is_valid;
  float temperature;
  uint32 timestamp;
} therm_reading_t;

/*
  void therm_init()

  Thermometer module initialization.
*/
void therm_init();

/*
  void therm_deinit()

  Thermometer module de-initialization.
*/
void therm_deinit();

/*
    void therm_capture()
    
    Initialiate a temperature reading with
    the thermometer.  This is a non-blocking call.
    The caller will need to ensure a wait of 750us
    has elapsed before reading back the temperature.
    See therm_read.
*/
void therm_capture();

/*
  void therm_read()

  This reads out the capture temperature
  reading from the last therm_capture() call.

  The temperature is returned in therm_reading
  structure.  If the thermometer reading failed
  the valid flag in therm_reading is set to false.
*/
void therm_read(therm_reading_t *therm_reading);
  
#endif // THERM_H