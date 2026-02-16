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

#define SlaveAddress 0x30
#define FACTOR_CALIBRACION 45.0 // Ajustar según calibración real

volatile uint16_t pesoEnGramos = 0;
uint8_t buffer = 0;
uint8_t byteIndex = 0;

int main(void)
{
	DDRD |= (1 << DDD4); // Configuración pines
	
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
