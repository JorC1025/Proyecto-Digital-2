/*
 * I2C.h
 *
 * Created: 9/2/2026 22:48:11
 *  Author: jorge
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <stdint.h>

//********
//Inicializar I2C Master
//*********
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler);

//Inicia Comunicación I2C
uint8_t I2C_Master_Start(void);
uint8_t I2C_Master_Repeated_Start(void);

//Para Comunicación I2C
void I2C_Master_Stop(void);

//Transmisión de datos de maestro a esclavo
//Devuelve 0 si esclavo recibe info
uint8_t I2C_Master_Write(uint8_t dato);

//Transmisión de datos de esclavo a maestro
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);

//Iniciar Esclavo
void I2C_Slave_Init(uint8_t address);

#endif /* I2C_H_ */