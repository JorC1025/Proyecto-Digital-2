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

#define SlaveAddress 0x30

uint8_t buffer = 0;
volatile uint8_t distancia_cm = 0;
volatile uint8_t comando = 0;
volatile uint8_t dato_recibido = 0;
volatile uint8_t esperando_dato = 0;

void ultrasonico_init(void){
	DDRB |= (1<<PB3);   // TRIG salida
	DDRB &= ~(1<<PB4);  // ECHO entrada
	PORTB |= (1<<PB4);  // ?? PULL-UP INTERNA en ECHO (MUY IMPORTANTE)

	
}

uint16_t medir_us(void){
	 uint16_t timeout = 0;
	 
	PORTB &= ~(1<<PB3);
	_delay_us(2);
	
	PORTB |= (1<<PB3);
	_delay_us(10);
	PORTB &= ~(1<<PB3);
	
	while(!(PINB &(1<<PB4))){
		if(++timeout > 30000) return 0xFFFF;
		};
		
	
	TCNT1 = 0;
	TCCR1B = (1<<CS11); // prescaler 8

	timeout = 0;
	
	while(PINB & (1<<PB4)){
		if(++timeout > 60000) break;
		}; // Medir distancia

	TCCR1B = 0;
	return TCNT1/2;
	
}

uint8_t convertir_distancia(void){
	uint16_t t = medir_us();
	uint16_t d = t / 58;
	if(d > 255) d = 255;
	return (uint8_t)d;
}

int main(void)
{
	DDRD |= (1<<DDD4);
	DDRD |= (1<<DDD3);
    ultrasonico_init();
	I2C_Slave_Init(SlaveAddress);
	sei();
    while (1) 
    {
		if (buffer == 'R')
		{
			distancia_cm = convertir_distancia();
			if (distancia_cm < 15)
			{
				PORTD |= (1<<PD4);
			} else {
				PORTD &= ~(1<<PD4);
				
			}			
			buffer = 0;
		}
		//Procesamiento de nivel de luz
		
			if(buffer == 'L'){
				if(dato_recibido < 50){
					PORTD &= ~(1<<PD3);
				} else{
					PORTD |= (1<<PD3);
				}
				buffer = 0;
			}
		
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
			 if(!esperando_dato)
			 {
				 buffer = TWDR;
				 esperando_dato = 1;
			 }
			 else
			 {
				 dato_recibido = TWDR;
				 esperando_dato = 0;
			 }

			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0xA8:
		case 0xB8:
			TWDR = distancia_cm;
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
