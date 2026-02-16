/*
 * Proyecto1EsclavoUltrasonico.c
 *
 * Created: 9/2/2026 22:47:12
 * Author : Jorge Cruz - 23502
 * Erick Perez - 23502
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "PWM0/PWM0.h"

#define SlaveAddress 0x32



// Pines del driver
#define AIN1 PD0
#define AIN2 PD1
#define BIN1 PD2
#define BIN2 PD3



uint8_t buffer = 0;
volatile uint8_t distancia_cm = 0;

uint16_t pulso_us = 1000;
uint16_t pulse_us = 1000;
uint8_t duty = 63;
uint8_t cant_volt = 0;
uint8_t pulso_recibido = 0;
uint8_t contador = 0;
uint8_t pwmmanual = 0;

const uint8_t secuencia[4][4] = {
	{1,0,1,0}, // Paso 1
	{0,1,1,0}, // Paso 2
	{0,1,0,1}, // Paso 3
	{1,0,0,1}  // Paso 4
};

#define POS_3  0    // posición en pasos
#define POS_6  50   // ejemplo: 50 pasos desde las 3
#define POS_9  100  // ejemplo: 100 pasos desde las 3

volatile uint8_t posicion_actual = 0;
volatile uint8_t posicion_objetivo = 0;



void stepper_step(uint8_t paso) {
	if(secuencia[paso][0]) PORTD |= (1<<AIN1); else PORTD &= ~(1<<AIN1);
	if(secuencia[paso][1]) PORTD |= (1<<AIN2); else PORTD &= ~(1<<AIN2);
	if(secuencia[paso][2]) PORTD |= (1<<BIN1);  else PORTD &= ~(1<<BIN1); // BIN1 en PD3
	if(secuencia[paso][3]) PORTD |= (1<<BIN2);  else PORTD &= ~(1<<BIN2); // BIN2 en PD4
}

void stepper_update(void)
{
	if(posicion_actual < posicion_objetivo)
	{
		posicion_actual++;
		stepper_step(posicion_actual % 4);
		_delay_ms(10);
	}
	else if(posicion_actual > posicion_objetivo)
	{
		posicion_actual--;
		stepper_step(posicion_actual % 4);
		_delay_ms(10);
	}
}



int main(void)
{
	cli();


	pwm0_init();

	DDRC &= ~(1<<DDC3);
	DDRC &= ~(1<<DDC2);
	DDRC &= ~(1<<DDC1);
	DDRD |= (1<<DDD0);
	DDRD |= (1<<DDD1);
	DDRD |= (1<<DDD2);
	DDRD |= (1<<DDD3);
	DDRD |= (1<<DDD4);
	
	I2C_Slave_Init(SlaveAddress);
	sei();
	
	uint8_t paso = 0;

    while (1) 
    {
		if (buffer != 0)
		{
			 if(buffer <= 15)
			 { posicion_objetivo = POS_3;}
			 else if(buffer <= 50)
			{ posicion_objetivo = POS_6;}
			 else
			 { posicion_objetivo = POS_3;}

			 
			 buffer = 0;
			 
		}
					 stepper_update();
	/*
		//gira horario, solo era para probar secuencia lol
		stepper_step(paso);
		paso++;
		if(paso >= 4) paso = 0; // reinicia secuencia
		_delay_ms(100); // velocidad del giro

		*/
		
			/*	
			pwm para servo
			pulse_us = 1000 + ((uint32_t)buffer * 1000) / 50;

			if(pulse_us < 1000) pulse_us = 1000;
			if(pulse_us > 2000) pulse_us = 2000;

			pwm0_set_pulse(pulse_us);
			
			
		//driver dc tambien era pa probar
			if (buffer < 15)
			{
				PORTD |= (1 << AIN1);   // IN1 HIGH

			}else
			{

				PORTD &= ~(1 << AIN1); 

			}
*/
		
    }
}





ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC;
	switch(estado){
		
		case 0x60:
		case 0x70:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0x80:
		case 0x90:
			buffer = TWDR;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0xA8:
		case 0xB8:
			//pwmmanual = TWDR;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			 
		case 0xC0:
		case 0xC8:
			TWCR = 0;
			TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
			break;
			
		case 0xA0:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		default:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
	}
}

