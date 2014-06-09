#ifndef PWM_H
#define PWM_H

#include "types.h"

void pwm_init(uint32 freq);

void pwm_run();

void pwm_update_duty(uint32 duty_cycle);

void pwm_update_timer();

#endif
