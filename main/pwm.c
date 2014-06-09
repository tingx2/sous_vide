#include <bcm2835.h>
#include "pwm.h"
#include "types.h"

#define PIN RPI_GPIO_P1_11

static uint32 freq_hz; //Frequency for PWM

static uint32 time_high_thres;
static uint32 time_low_thres;

static uint32 time_high_thres_incre;
static uint32 time_low_thres_incre;

void pwm_init(uint32 freq)
{
  freq_hz=freq;
	int ret_val= bcm2835_init();
  // add error handling

	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
}

void pwm_run()
{
	uint32 cur_time = read_timer32();
	if(cur_time - time_high_thres < 0xFFFFFFFF/2)
	{
		pwm_timer_update();
		bcm2835_gpio_write(PIN, HIGH);
	}
	else if (cur_time - time_low_thres < 0xFFFFFFFF/2)
		bcm2835_gpio_write(PIN, LOW);

}

void pwm_timer_update()
{	
	uint32 cur_time = read_timer32(); 
	time_high_thres= cur_time + time_high_thres_incre; //Toggle LOW to HIGH end of period.
	time_low_thres= cur_time + time_low_thres_incre; //Toggle HIGH to LOW end of period.
}

void pwm_update_duty(uint32 duty_cycle)
{
	time_high_thres_incre= (100000/freq_hz); 
	time_low_thres_incre= (1000/freq_hz*duty_cycle); 
}
