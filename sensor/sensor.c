#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bcm2835.h>

typedef unsigned char uint8;
typedef unsigned long long uint64;
typedef unsigned short uint16;

#define SENSOR_GPIO_PIN					RPI_GPIO_P1_07

#define DELAY1MS()								\
  do {														\
			for(int ii=0;ii<100; ii++); \
	}while(0)

bool sensor_handshake();

bool sensor_read()
{

	bool bit;
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
//  bcm2835_gpio_set_pud(SENSOR_GPIO_PIN, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);

	return bit;
}
bool sensor_bit_out(void)
{
	bool bit;
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_clr(SENSOR_GPIO_PIN);
  DELAY1MS();
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
  DELAY1MS();
  bit = bcm2835_gpio_lev(SENSOR_GPIO_PIN);

//  bcm2835_gpio_lev(SENSOR_GPIO_PIN);
//	usleep(5);
	
	return bit;
}

uint8 sensor_byte_out(void)
{
	uint8 i;
	uint8 value = 0;
	
	for(i=0; i<8; i++)
	{
		if(sensor_bit_out())
		{
			value |= (1 << i);
		}
		usleep(0);
	}
	return value;
}


void sensor_bit_in(bool bit)
{
  bcm2835_gpio_clr(SENSOR_GPIO_PIN);
  DELAY1MS();
	if(bit)
	{
  	bcm2835_gpio_set(SENSOR_GPIO_PIN);
	}
	usleep(0);
  bcm2835_gpio_set(SENSOR_GPIO_PIN);
//  printf("bit:%d\n", bit);
}

void sensor_byte_in(uint8 val)
{
	uint8 i;
	uint8 temp;
	
	usleep(20);
	for(i=0; i<8; i++)
	{
		temp = val >> i;
		temp &= 0x01;
		sensor_bit_in(temp);
		DELAY1MS();
	}
	//printf("%x\n",val);
}

void find_device(void)
{
	
	uint8 rom_addr[8];
	uint8 byteid=0;
	uint8 mask = 0x1;
	uint8 bitpos = 1;
	uint8 conflict_prev = 0;
  uint8 state;
  uint8 rom_data[8];
  uint64 rom_val=0;
	bool bit;

  sensor_handshake();
	
	sensor_byte_in(0x33);
  DELAY1MS();
	for(int i=0; i<8; i++)
	{
	//	rom_data[i]=sensor_byte_out();
		rom_val |= ((uint64)(sensor_byte_out()) << (i*8));
	}
	printf("rom_val:0x%llx\n",rom_val);
	
#if 0
	for(int i=0; i<8; i++)
	{
		//rom_data[i]=sensor_byte_out();
		printf("rom_val:0x%x\n",rom_data[i]);
		//rom_val |= ((uint64)(sensor_byte_out()) << (i*8));
	}
	
if(sensor_bit_out()!=0)
	{
		state = 2;
	}
	usleep(120);

	if(sensor_bit_out()!=0)
	{
		state |= 1;
	}
	usleep(120);
	
	byteid = 0;
 	do
  {
        /* read bit */
        if ( sensor_bit_out() != 0 )
        {
            state = 2;
        }
        usleep(120);

        /* read bit complement */
        if ( sensor_bit_out() != 0 )
        {
            state |= 1;
        }
        usleep(120);

        /* description for values of state: */
        /* 00    There are devices connected to the bus which have conflicting */
        /*       bits in the current ROM code bit position. */
        /* 01    All devices connected to the bus have a 0 in this bit position. */
        /* 10    All devices connected to the bus have a 1 in this bit position. */
        /* 11    There are no devices connected to the 1-wire bus. */

	//printf("state: %d\n", state);
        /* if there are no devices on the bus */
        if (state == 3)
        {
            break;
        }
        else
        {
            /* devices have the same logical value at this position */
            if (state > 0)
            {
                /* get bit value */
                bit = (bool)(state >> 1);
            }
            /* devices have confilcting bits in the current ROM code */
            else
            {
                /* if there was a conflict on the last iteration */
                if (bitpos < conflict_prev)
                {
                    /* take same bit as in last iteration */
                    bit = ( (rom_addr[byteid] & mask) > 0 );
                }
                else
                {
                    bit = (bitpos == conflict_prev);
                }

                if (bit == 0)
                {
                    conflict_prev = bitpos;
                }
            }

            /* store bit in ROM address */
           if (bit != 0)
           {
               rom_addr[byteid] |= mask;
           }
           else
           {
               rom_addr[byteid] &= ~mask;
           }

           sensor_bit_in(bit);

           /* increment bit position */
           bitpos ++;

           /* calculate next mask value */
           mask = mask << 1;

           /* check if this byte has finished */
           if (mask == 0)
           {
               byteid ++;  /* advance to next byte of ROM mask */
               mask = 1;    /* update mask */
           }
        }
		//		printf("bitpos: %d \n",bitpos);
    } while (byteid < 8);
#endif
	 	//printf("romADDR:");
		//for(int i =0; i<8; i++)
		//	printf("%d", rom_addr[i]);
}


int main()
{
  uint64 rom_val=0;
	bcm2835_init();
  //bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);
  //bcm2835_gpio_set_pud(SENSOR_GPIO_PIN, BCM2835_GPIO_PUD_UP);
//	find_device();

while(true)
{

				
	uint8 i;
	uint8 scrpad[9];

	sensor_handshake();
	sensor_byte_in(0xCC);
	sensor_byte_in(0x44);
  //bcm2835_gpio_set(SENSOR_GPIO_PIN);
	usleep(800000);

	sensor_handshake();
	sensor_byte_in(0xCC);
	sensor_byte_in(0xBE);
	DELAY1MS();
	for(int i=0; i<9; i++)
	{
		scrpad[i]=sensor_byte_out();
		//rom_val |= ((uint64)(sensor_byte_out()) << (i*8));
	}
	for(int i=8; i>=0; i--)
	{
		//scrpad[i]=sensor_byte_out();
		printf("%2x",scrpad[i]);
		//rom_val |= ((uint64)(sensor_byte_out()) << (i*8));
	}
	float temp = (float)((uint16)scrpad[0] + ((uint16)(scrpad[1]&0xf)<<0x8)); 
	printf("temp:%f\n", temp);
	temp = temp * 0.0625; 
	printf("temp:%f\n", temp);
/*	

	for(i=0; i<9; i++)
	{
		scrpad[i] = sensor_byte_out();
  }
	printf("temp:");
	for(i=0; i<9; i++)
	{
		printf("%d",scrpad[i]);
  }*/
}
	return 0;
}

bool sensor_handshake()
{
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
	usleep(1000);
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);

//  bcm2835_gpio_set_pud(SENSOR_GPIO_PIN, BCM2835_GPIO_PUD_UP);
	bool handshake;
  bcm2835_gpio_clr(SENSOR_GPIO_PIN);
	//int resPulse = bcm2835_gpio_lev(SENSOR_GPIO_PIN);
	//printf("low: %d ", resPulse);
	usleep(480);
  bcm2835_gpio_set(SENSOR_GPIO_PIN);
	//resPulse = bcm2835_gpio_lev(SENSOR_GPIO_PIN);
//	printf("high: %d ", resPulse);
	usleep(40);
  
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_INPT);
	handshake = bcm2835_gpio_lev(SENSOR_GPIO_PIN);
	//printf("handshake: %d \n", handshake);
	usleep(480);
  bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);

  //bcm2835_gpio_fsel(SENSOR_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);
  return handshake;
}

