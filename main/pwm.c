#include <bcm2835.h>
#include "pwm.h"
#include "types.h"

#define PIN RPI_GPIO_P1_11

static uint32 freq_hz; //Frequency for PWM

static uint32 time_high_thres;
static uint32 time_low_thres;


void pwm_init()
{
  freq_hz=100;
	int ret_val= bcm2835_init();
  // add error handling

	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);;
}

void pwm_run(uint32 duty_cycle)
{
	int cur_time = read_timer32(); 
	if(cur_time>time_high_thres)
	{
		pwm_update_duty(duty_cycle);
		bcm2835_gpio_write(PIN, HIGH);
	}
	else if (cur_time>time_low_thres)
		bcm2835_gpio_write(PIN, LOW);

}

void pwm_update_duty(uint32 duty_cycle)
{
	int cur_time = read_timer32(); 
	time_high_thres= cur_time+(100000/freq_hz); //Toggle LOW to HIGH end of period.
	time_low_thres= cur_time+(1000/freq_hz*duty_cycle); //Toggle LOW to HIGH end of period.
}
