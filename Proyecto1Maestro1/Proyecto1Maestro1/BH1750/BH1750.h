/*
 * BH1750.h
 * Librería simple para sensor de luz I2C
 */ 

#ifndef BH1750_H_
#define BH1750_H_

#include <avr/io.h>
#include <stdint.h>
// Importante: Necesita tu librería I2C básica

// Dirección I2C (Si ADDR=GND -> 0x23, Si ADDR=VCC -> 0x5C)
#define BH1750_ADDR 0x23

// Comandos de Operación
#define BH1750_POWER_ON 0x01
#define BH1750_RESET    0x07
#define BH1750_CONTINUOUS_HIGH_RES_MODE 0x13 // 1 lux resolución, 120ms tiempo medida

// Macros para Lectura/Escritura (Desplazamiento para el bit R/W)
#define BH1750_WRITE ((BH1750_ADDR << 1) & 0xFE)
#define BH1750_READ  ((BH1750_ADDR << 1) | 0x01)

// Prototipos de funciones
void BH1750_Init(void);
uint16_t BH1750_ReadLux(void);

#endif /* BH1750_H_ */