#include "UART.h"

void initUART(void){

	DDRD |= (1 << DDD1);  // TX como Salida
	DDRD &= ~(1 << DDD0); // RX como Entrada

	// UBRR = 103 equivale a 9600 baudios con cristal de 16MHz
	UCSR0A = 0; // Velocidad normal
	UBRR0 = 103;

	// Habilitar RX y TX
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void writeChar(char caracter){
	// Esperar a que esté vacío (UDRE0 = 1)
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Poner el dato en el registro UDR0 para enviarlo
	UDR0 = caracter;
}

void writeString(char* texto){
	// Recorrer la cadena hasta encontrar el caracter nulo
	for (uint8_t i = 0; texto[i] != '\0'; i++){
		writeChar(texto[i]);
	}
}

char readChar(void){
	// Esperar a que llegue un dato
	while (!(UCSR0A & (1 << RXC0)));
	
	// Devolver el dato que llegó
	return UDR0;
}

uint8_t UART_HayDato(void) {
	// Revisa si el bit RXC0 está en 1
	if (UCSR0A & (1 << RXC0)) {
		return 1;
		} else {
		return 0;
	}
}
