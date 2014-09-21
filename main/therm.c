/*
  therm.c
  
  Driver for DS1820 Sensor (1 Wire Dallas protocol)

*/


/**********************************************************************

  Header Files

**********************************************************************/
#include <stdio.h>
#include "types.h"
#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "therm.h"

/* -------------------------------------------------------------------------- */
/*                         DS1820 Timing Parameters                           */
/* -------------------------------------------------------------------------- */
/* Generate a delay in useconds since
   usleep call is not accurate enough for
   <80us sleep durations */
#define THERM_DELAY_US(usec) do { for(uint32 ii = 0; ii < (100*usec); ii++); } while(0);

#define DS1820_RST_PULSE       480   /* master reset pulse time in [us] */
#define DS1820_MSTR_BITSTART   2     /* delay time for bit start by master */
#define DS1820_PRESENCE_WAIT   40    /* delay after master reset pulse in [us] */
#define DS1820_PRESENCE_FIN    480   /* dealy after reading of presence pulse [us] */
#define DS1820_BITREAD_DLY     5     /* bit read delay */
#define DS1820_BITWRITE_DLY    100   /* bit write delay */
#define DS1820_READ_WAIT_IN_NS 750000 /* Wait 750us after issuing a capture command
                                         before reading back the temperature. */

/* -------------------------------------------------------------------------- */
/*                            DS1820 Registers                                */
/* -------------------------------------------------------------------------- */
#define DS1820_REG_TEMPLSB    0
#define DS1820_REG_TEMPMSB    1
#define DS1820_REG_CNTREMAIN  6
#define DS1820_REG_CNTPERSEC  7
#define DS1820_ADDR_LEN       8

#define DS1820_SCRPADMEM_CRC_BYTE 8   /* Byte index for the CRC */
#define DS1820_SCRPADMEM_LEN  9       /* Length of scratchpad memory */

/* -------------------------------------------------------------------------- */
/*                            DS1820 Commands                                 */
/* -------------------------------------------------------------------------- */
#define DS1820_CMD_SEARCHROM     0xF0
#define DS1820_CMD_READROM       0x33
#define DS1820_CMD_MATCHROM      0x55
#define DS1820_CMD_SKIPROM       0xCC
#define DS1820_CMD_ALARMSEARCH   0xEC
#define DS1820_CMD_CONVERTTEMP   0x44
#define DS1820_CMD_WRITESCRPAD   0x4E
#define DS1820_CMD_READSCRPAD    0xBE
#define DS1820_CMD_COPYSCRPAD    0x48
#define DS1820_CMD_RECALLEE      0xB8

#define DS1820_MAX_NUM_READ_RETRIES		2

#define DS1820_FAMILY_CODE_DS18B20      0x28
#define DS1820_FAMILY_CODE_DS18S20      0x10

/**********************************************************************

  Definitions/Variables

**********************************************************************/

/* This keeps track of the state of the driver.
   
   THERM_CAPTURE_READY = lastest temperature is ready to be read back
   THERM_CAPTURE_IN_PROGRESS = need to wait for the capture to complete */
 typedef enum
 {
   THERM_CAPTURE_READY,
   THERM_CAPTURE_WAITING,
 } therm_capture_state_e;

 /* Thermometer capture state */
static therm_capture_state_e therm_capture_state;

/* The system timestamp when the last temperature
   read was issued. */
static uint32 therm_last_timestamp;

/* This is the 9 byte scratchpad which is read back from the device. */
static uint8 scrpad[DS1820_SCRPADMEM_LEN];

/**********************************************************************

  Fucntions

**********************************************************************/

/*
  uint8 crc8(uint8 crc, uint8 data) 
  
  Helper function for CRC calculation.
*/
static uint8 crc8(uint8 crc, uint8 data) 
{ 
  crc = crc ^ data;

  for (uint32 i = 0; i < 8; i++) 
  { 
    if (crc & 0x1) 
    {
      crc = (crc >> 1) ^ 0x8C;
    }
    else
    {
      crc >>= 1;
    }
  }

  return crc;
} 

/*
  uint8 crc_calc(uint8 *p)

  This calculates the 8-bit CRC value to be
  compared with the value read back from the device
  scratchpad (byte 8).  If they match, it means the
  temperature read back is valid.
*/
static uint8 crc_calc(uint8 *p)
{
  uint8 crc = 0;

  for (uint32 i = 0; i < 8; i++)
  {
    crc = crc8(crc, *p++);
  }

  return crc;
}

/*
  boolean therm_handshake()
  
  Resets sensor flow to start state.
  This preceeds each new command to the sensor.

  The return value signifies if the
  handshake was successful or not.

  The sequence is as follows:
  1. Toggle GPIO low
  2. Wait 480us
  3. Toggle GPIO high
  4. Wait for 40us
  5. Read GPIO back
  6. Wait 480us

*/
static boolean therm_handshake()
{
  boolean handshake;  

  GPIO_DIRECTION_OUTPUT(GPIO_THERM);
  GPIO_CLR(GPIO_THERM);
  THERM_DELAY_US(DS1820_PRESENCE_FIN);
  GPIO_SET(GPIO_THERM);
  THERM_DELAY_US(DS1820_PRESENCE_WAIT);
  GPIO_DIRECTION_INPUT(GPIO_THERM);
  handshake = (boolean)GPIO_READ(GPIO_THERM);
  THERM_DELAY_US(DS1820_PRESENCE_FIN);

  return handshake;
}

/*
  uint8 therm_bit_in(void)
  
  Issue a "read-slot" and reads back
  a single bit from the thermometer.

  The sequence is as follows:
  1. Toggle GPIO low
  2. Wait 1us
  3. Set GPIO to input (let it float)
  4. Read GPIO back

*/
static uint8 therm_bit_in(void)
{
  uint8 bit = 0;

  GPIO_DIRECTION_OUTPUT(GPIO_THERM);
  GPIO_CLR(GPIO_THERM);
  THERM_DELAY_US(1);
  GPIO_DIRECTION_INPUT(GPIO_THERM);
  THERM_DELAY_US(1);
  bit = GPIO_READ(GPIO_THERM);  

  return bit;
}

/*
  uint8 therm_byte_in(void)

  Reads a single byte back from the
  thermometer device.

  There is a 80us delay between
  reading each bit.
*/
static uint8 therm_byte_in(void)
{
  uint8 value = 0;
  
  for (uint32 i = 0; i < 8; i++)
  {
    if (therm_bit_in())
    {
      value |= (1 << i);
    }
    THERM_DELAY_US(80);
  }

  return value;
}

/*
  void therm_bit_out(uint8 bit)

  Issue a "write-slot" and sends a
  single bit to the thermometer
  device.

  The sequence is as follows:
  1. Toggle GPIO low
  2. Wait 1us
  3. If bit to be sent is 1, toggle GPIO high
  4. Wait for 80us
  5. Toggle the GPIO high

*/
static void therm_bit_out(uint8 bit)
{
  GPIO_DIRECTION_OUTPUT(GPIO_THERM);
  GPIO_CLR(GPIO_THERM);
  THERM_DELAY_US(1);
  
  if (bit)
  {
    GPIO_SET(GPIO_THERM);
  }

  THERM_DELAY_US(80); 
  GPIO_SET(GPIO_THERM);
}

/*
  void therm_byte_out(uint8 byte)
  
  Sends a single byte to the thermometer
  device.

  There is a 1us delay between sending
  each bit.

*/
static void therm_byte_out(uint8 byte)
{  
  THERM_DELAY_US(80);
  for (uint32 i = 0; i < 8; i++)
  {
    therm_bit_out((byte >> i) & 0x1);
    THERM_DELAY_US(1);
  }
}

/*
  boolean therm_scrpad_rd()
  
  Reads out 9 bytes from the device scratch pad, 
  called after issuing a prior request command.

  A scratchpad read is issued by sending:
  1. SKIPROM command
  2. READSCRAPAD command

*/
static void therm_scrpad_rd()
{
  boolean failed = therm_handshake();
  assert(!failed);

  therm_byte_out(DS1820_CMD_SKIPROM);
  therm_byte_out(DS1820_CMD_READSCRPAD);

  THERM_DELAY_US(1);

  for (uint32 i = 0; i < DS1820_SCRPADMEM_LEN; i++)
  {
    scrpad[i] = therm_byte_in();
  }
}

/*
  boolean therm_read_internal()
  
  Reads out 9 bytes from the device scratch pad, 
  called after issuing a prior request command.

  A scratchpad read is issued by sending:
  1. SKIPROM command
  2. READSCRAPAD command

  Returns TRUE if data is valid, otherwise
  returns false.
*/
static boolean therm_read_internal(therm_reading_t *therm_reading)
{
  /* Read the scratchpad from the device. */
  therm_scrpad_rd();

  /* Calculate CRC. */
  uint8 crc = crc_calc(scrpad);

  if ( crc != scrpad[DS1820_SCRPADMEM_CRC_BYTE] )
  {
    therm_reading->is_valid = FALSE;
    DEBUG_MSG_LOW(DEBUG_MODULE_THERM, "CRC (calculated)0x%x != 0x%x(scratchpad)",
                  crc, scrpad[DS1820_SCRPADMEM_CRC_BYTE]);   
    return FALSE;
  }

  return TRUE;
}

/*
  void therm_capture()
  
  API call to tell sensor to start temperature conversion. 
  A subsequent call should be made 750ms after to read the
  captured temperature.
*/
void therm_capture()
{
  if (therm_capture_state != THERM_CAPTURE_READY)
  {
    return;
  }

  boolean failed = therm_handshake();
  assert(!failed);

  therm_byte_out(DS1820_CMD_SKIPROM);
  therm_byte_out(DS1820_CMD_CONVERTTEMP);
  therm_last_timestamp = timer_read32();
  
  therm_capture_state = THERM_CAPTURE_WAITING;

  DEBUG_MSG_HIGH(DEBUG_MODULE_THERM, "therm_capture() sent.");
}

/*

  void therm_read(therm_reading_t *therm_state)

  API called to read the temperature data from device.
  This should be called at least 750us after therm_capture()
  is issued.
  
*/
void therm_read(therm_reading_t *therm_reading)
{
  if (therm_capture_state != THERM_CAPTURE_WAITING)
  {
    return;
  }

  uint32 dummy_time;
  boolean is_capture_done =
    timer_is_elapsed32( (therm_last_timestamp + DS1820_READ_WAIT_IN_NS), &dummy_time);
  if (!is_capture_done)
  {
  	return;
  }

  therm_reading->is_valid = FALSE;
  therm_reading->temperature = 0;
  therm_reading->timestamp = 0;

  therm_capture_state = THERM_CAPTURE_READY;

  boolean is_successful = FALSE;
  uint32 num_tries = 0;
  do{
  	num_tries++;
    is_successful = therm_read_internal(therm_reading);
  } while( (!is_successful) && (num_tries < DS1820_MAX_NUM_READ_RETRIES) );

  if (num_tries == DS1820_MAX_NUM_READ_RETRIES)
  {
    DEBUG_MSG_MED(DEBUG_MODULE_THERM, "Temperature read failed.");
  	return;
  }

  therm_reading->is_valid = TRUE;
  therm_reading->timestamp = therm_last_timestamp;
  therm_reading->temperature = ( (float)((uint32)scrpad[DS1820_REG_TEMPLSB] +
                                 ((uint32)(scrpad[DS1820_REG_TEMPMSB]&0xf)<<0x8)) ) * 0.0625;

  DEBUG_MSG_MED(DEBUG_MODULE_THERM, "Temperature: %4.4f celsius ", therm_reading->temperature);
}

/*
  therm_init()
  
  Initializes the therm module.
*/

void therm_init()
{
  therm_capture_state = THERM_CAPTURE_READY;
  DEBUG_MSG_LOW(DEBUG_MODULE_THERM, "therm_init() complete.");
}

/*
  therm_deinit()
  
  De-initializes the therm module.
*/

void therm_deinit()
{	
  DEBUG_MSG_LOW(DEBUG_MODULE_THERM, "therm_deinit() complete.");
}