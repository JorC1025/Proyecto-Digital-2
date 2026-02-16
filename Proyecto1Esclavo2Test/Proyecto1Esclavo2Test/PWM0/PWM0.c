/*
 * PWM0.c
 *
 * Created: 8/4/2025 15:09:57
 *  Author: Jorge Cruz
 */ 
#include "PWM0.h"

void pwm0_init(){

// Configurar PB1 (OC1A) como salida
DDRB |= (1 << DDB1);

// Modo Fast PWM con TOP = ICR1
TCCR1A = (1 << COM1A1) | (1 << WGM11);  // Se limpia el 0CR1A por comparación
TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8

ICR1 = 40000;  // TOP para 20ms (50Hz) con F_CPU = 16MHz y prescaler = 8


}

void pwm0_set_pulse(uint16_t pulse_us) {
	// pulse_us en microsegundos (por ejemplo 1000 a 2000)
	// Convertir a ticks del timer (16 MHz / 8 = 2 MHz ? 1 tick = 0.5 us)

	
	OCR1A = (pulse_us)*2;
}