/*
 * PWM0.h
 *
 * Created: 8/4/2025 15:11:09
 *  Author: jorge
 */ 


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>

void pwm0_init();
void pwm0_set_pulse(uint16_t pulse_us);




#endif /* PWM0_H_ */