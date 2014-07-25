/*
	therm.c
	
	Driver for DS1820 Sensor (1 Wire Dallas protocol)

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bcm2835.h>

/* -------------------------------------------------------------------------- */
/*                         DS1820 Timing Parameters                           */
/* -------------------------------------------------------------------------- */

#define DS1820_RST_PULSE       480   /* master reset pulse time in [us] */
#define DS1820_MSTR_BITSTART   2     /* delay time for bit start by master */
#define DS1820_PRESENCE_WAIT   40    /* delay after master reset pulse in [us] */
#define DS1820_PRESENCE_FIN    480   /* dealy after reading of presence pulse [us] */
#define DS1820_BITREAD_DLY     5     /* bit read delay */
#define DS1820_BITWRITE_DLY    100   /* bit write delay */


/* -------------------------------------------------------------------------- */
/*                            DS1820 Registers                                */
/* -------------------------------------------------------------------------- */

#define DS1820_REG_TEMPLSB    0
#define DS1820_REG_TEMPMSB    1
#define DS1820_REG_CNTREMAIN  6
#define DS1820_REG_CNTPERSEC  7
#define DS1820_SCRPADMEM_LEN  9     /* length of scratchpad memory */

#define DS1820_ADDR_LEN       8


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


#define DS1820_FAMILY_CODE_DS18B20      0x28
#define DS1820_FAMILY_CODE_DS18S20      0x10

// can replace with types.h after integrating
typedef unsigned char uint8;
typedef unsigned long long uint64;
typedef unsigned short uint16;

static uint8 scrpad[9] = {};

// make new gpio.h file, define all used gpios there have all modules include gpio.h
#define SENSOR_GPIO_PIN					RPI_GPIO_P1_07

// change to THERM_DELAY_IN_US macro
void therm_delay_us(int usec)
{
	for(int i=0; i<usec*100; i++);
}

/*
	therm_init()
	
	calls bcm2835 init to intialize APIs to toggle GPIOs
*/

void therm_init()
{
	uint32 ret_val = bcm2835_init();
	if(ret_val==0)
		DEBUG_MSG_ERROR("therm_init::bcm2835_init() failed.");
		
	DEBUG_MSG_NOTIME(SENSOR_DEBUG_PWM, "therm_init() complete.");
}

/*
	
	uint8 crc8(uint8 crc, uint8 data) 
	
	Follows the same method to calculate checksum as the DS1820 sensor

*/


uint8 crc8(uint8 crc, uint8 data) 
{ 
	uint8 i;
	crc = crc ^ data; 
	for (i = 0; i < 8; i++) 
	{ 
		if (crc & 0x01) 
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
	
	uint8 crc_calc(uint8 *p, int length)

	Wrapper function to called by temp_read to calculate CRC from reading scratchpad data	

*/


uint8 crc_calc(uint8 *p, int length)
{
	uint8 crc = 0;
	while (length--) 
	{
		crc = crc8(crc, *p++);
	}
	return crc;
}

/*
	
	bool therm_handshake()
	
	Resets sensor flow to start state, done everytime before a new command is issued

*/


bool therm_handshake()
{
	bool handshake;  

  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);

 	bcm2835_gpio_clr(SENSOR_GPIO_PIN);	
	therm_delay_us(DS1820_PRESENCE_FIN);
  
	bcm2835_gpio_set(SENSOR_GPIO_PIN);
	therm_delay_us(DS1820_PRESENCE_WAIT);
  
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
	handshake = bcm2835_gpio_lev(SENSOR_GPIO_PIN);
	
	therm_delay_us(DS1820_PRESENCE_FIN);

  return handshake;
}

/*

	bool therm_bit_in(void)
	
	Creates a read window for sensor and read back 1 bit

*/


bool therm_bit_in(void)
{
	bool bit;
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_clr(SENSOR_GPIO_PIN);
	therm_delay_us(1);
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
	therm_delay_us(1);
  bit = bcm2835_gpio_lev(SENSOR_GPIO_PIN);
	
  return bit;
}

/*

	bool therm_byte_in(void)

	Compiles return values from therm_bit_in into a byte	

*/


uint8 therm_byte_in(void)
{
	uint8 i;
	uint8 value = 0;
	
	for(i=0; i<8; i++)
	{
		if(therm_bit_in())
		{
			value |= (1 << i);
		}
		therm_delay_us(80);
	}
	return value;
}

/*

	bool therm_bit_out(bool_bit)

	Creates write window and sends a bit	

*/

void therm_bit_out(bool bit)
{
  bcm2835_gpio_clr(SENSOR_GPIO_PIN);
  therm_delay_us(1);
	if(bit)
	{
  	bcm2835_gpio_set(SENSOR_GPIO_PIN);
	}
	therm_delay_us(80);
  bcm2835_gpio_set(SENSOR_GPIO_PIN);
}

/*
	void therm_byte_out(uint8 val)
	
	Makes multiple calls to therm_bit_out to construct a byte

*/
void therm_byte_out(uint8 val)
{
	uint8 i;
	uint8 temp;
	
	therm_delay_us(100);
	for(i=0; i<8; i++)
	{
    therm_bit_out((val >> i) & 0x1);
		therm_delay_us(1);
	}
}

/*

	bool therm_capture()
	
	API call to tell sensor to start temperature conversion. A subsequent call should be made 750ms after to read data

*/

bool therm_capture()
{

	therm_handshake();
	therm_byte_out(DS1820_CMD_SKIPROM);
	therm_byte_out(DS1820_CMD_CONVERTTEMP);
	
	DEBUG_MSG_NOTIME(DEBUG_MODULE_THERM, "therm_capture() complete.");  
}

/*

	bool therm_scrpad_rd()
	
	Reads out 9 bytes from the device scratch pad, called after issuing a prior request command

*/


void therm_scrpad_rd()
{
	therm_handshake();
	therm_byte_out(DS1820_CMD_SKIPROM);
	therm_byte_out(DS1820_CMD_READSCRPAD);

	therm_delay_us(1);

	for(int i=0; i<9; i++)
	{
		scrpad[i]=therm_byte_in();
	}	
}

/*

	void therm_read(therm_state_t *therm_state)

	API called to read the temperature data from device, will try twice in case of communication failures
	
*/

void therm_read(therm_state_t *therm_state)
{
	uint8 crc;

	do
	{
		therm_state->valid = false;
		therm_scrpad_rd();
		
		crc = crc_calc(scrpad, 8);
		if (crc == scrpad[8])
		{
			DEBUG_MSG_NOTIME(DEBUG_MODULE_THERM, "CRC Correct");
			therm_state->temp = ((float)((uint16)scrpad[0] + ((uint16)(scrpad[1]&0xf)<<0x8))) * 0.0625; 
			therm_state->valid = true;
			return;
		}
		try++;
	}while(try < 2);
		
	DEBUG_MSG_NOTIME(DEBUG_MODULE_THERM, "CRC incorrect, temp read failed");
	
}
