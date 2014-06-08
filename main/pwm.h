#ifndef PWM_H
#define PWM_H

#include "types.h"

void pwm_init();

void pwm_run(uint32 duty_cycle);

void pwm_update_duty(uint32 duty_cycle);

#endif
