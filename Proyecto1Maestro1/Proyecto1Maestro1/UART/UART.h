#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

void initUART(void);
void writeChar(char caracter);
void writeString(char* texto);
char readChar(void);
uint8_t UART_HayDato(void);

#endif
