/*
 * Proyecto1EsclavoPeso.c
 *
 *
 * Created: 9/2/2026 22:47:12
 * Author : Jorge Cruz - 23502
 * Erick Perez - 23001
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "HX711/HX711.h"
#include "PWM0/PWM0.h"

#define SlaveAddress 0x32
#define FACTOR_CALIBRACION 45.0 // Ajustar según calibración real

//Variables para Stepper
volatile uint16_t pesoEnGramos = 0;
uint8_t buffer = 0;
uint8_t byteIndex = 0;
uint8_t pos_lista = 0;
long nueva_pos = 0;
//Variables para servo
uint16_t pulso_us = 1000;
uint16_t pulse_us = 1000;
uint8_t duty = 63;
uint8_t cant_volt = 0;
uint8_t pulso_recibido = 0;
uint8_t contador = 0;
uint8_t pwmmanual = 0;


const uint8_t secuencia[4] = {
	0b0011, //Paso 1
	0b0110, //Paso 2
	0b1100, //Paso 3
	0b1001  // Paso 4
};


volatile long posicion_actual = 0;
volatile uint8_t posicion_objetivo = 0;



void stepper_step(uint8_t direccion) {
	static uint8_t paso = 0;

	if(direccion > 0)
	{
		paso++;
		if(paso >= 4) paso = 0;
	}
	else
	{
		if(paso == 0) paso = 3;
		else paso--;
	}

	PORTD = (PORTD & 0x0F) | (secuencia[paso] << 4);
_delay_ms(3);
}

void stepper_update(long destino)
{
	long diferencia = destino - posicion_actual;

	if(diferencia > 0)
	{
		for(long i = 0; i < diferencia; i++)
		{
			stepper_step(1);
		}
	}
	else if(diferencia < 0)
	{
		for(long i = 0; i < -diferencia; i++)
		{
			stepper_step(-1);
		}
	}

	posicion_actual = destino;
	
}



int main(void)
{
	DDRD |= (1 << DDD4) |(1 << DDD5)| (1 << DDD6) |(1 << DDD7); // Configuración pines
	pwm0_init();
	I2C_Slave_Init(SlaveAddress);
	HX711_init();
	sei();
	
	// Tara inicial (20 lecturas)
	int32_t offset = HX711_readAverage_A128(20);
	
	while (1)
	{
		// Lectura y cálculo
		int32_t lecturaRaw = HX711_readAverage_A128(2);
		float calculo = (float)(lecturaRaw - offset) / FACTOR_CALIBRACION;
		
		if (calculo < 0) calculo = 0;
		
		// Actualización atómica de variable global
		cli();
		pesoEnGramos = (uint16_t)calculo; 
		sei();
		
		_delay_ms(10);
		
		//Entonces aqui va lo del motor
		
		
		if (pesoEnGramos < 15)
		{
		nueva_pos = 0;
		} else if (pesoEnGramos < 50)
		{
			nueva_pos = 512;
		} else {
			nueva_pos = 1024;
		}
		
		stepper_update(nueva_pos);
		//pwm0_set_pulse(1000);
		//pwm0_set_pulse(2000);
		//Todo arriba de esto funciona, por si hay que borrar es de aqui para abajo
		/*
		pos_lista = 1;
		*/

		
	
	
		
		/*
		cuando terminen de llegar a su posicion envian un uno a una variable nueva
		si esta variable nueva es 1 el servo usa pwm para ir de 0 a 180.
		*/
	}
}

ISR(TWI_vect){
	uint8_t estado = TWSR & 0xF8;
	
	switch(estado){
		
		case 0x60: // SLA+W recibido
		case 0x70:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;
		
		case 0x80: // Dato recibido
		case 0x90:
		buffer = TWDR;
		if (buffer == 'R') byteIndex = 0;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;
		
		case 0xA8: // SLA+R: Enviar Byte Alto (MSB)
		TWDR = (pesoEnGramos >> 8) & 0xFF;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;
		
		case 0xB8: // Data TX ACK: Enviar Byte Bajo (LSB)
		TWDR = (pesoEnGramos) & 0xFF;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;
		
		case 0xC0: // NACK / Fin
		case 0xC8:
		case 0xA0: // Stop
		default:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;
	}
}
