/*
 * BH1750.c
 */ 
#define F_CPU 16000000
#include "BH1750.h"
#include <util/delay.h>
#include "I2C/I2C.h"

void BH1750_Init(void)
{
    // 1. Encender el dispositivo
    I2C_Master_Start();
    I2C_Master_Write(BH1750_WRITE);
    I2C_Master_Write(BH1750_POWER_ON);
    I2C_Master_Stop();
    
    _delay_ms(10); // Pequeña espera

    // 2. Configurar modo continuo de alta resolución
    I2C_Master_Start();
    I2C_Master_Write(BH1750_WRITE);
    I2C_Master_Write(BH1750_CONTINUOUS_HIGH_RES_MODE);
    I2C_Master_Stop();
    
    // El sensor necesita ~180ms máx para la primera lectura
    _delay_ms(200); 
}

uint16_t BH1750_ReadLux(void)
{
    uint8_t buffHigh, buffLow;
    uint16_t lux_val;

    // Iniciar lectura
    I2C_Master_Start();
    I2C_Master_Write(BH1750_READ); // Dirección + Read
    
    // Leer Byte Alto con ACK (queremos otro byte)
    I2C_Master_Read(&buffHigh, 1);
    
    // Leer Byte Bajo con NACK (ya no queremos más)
    I2C_Master_Read(&buffLow, 0);
    
    I2C_Master_Stop();

    // Convertir bytes a valor Lux
    // Fórmula del datasheet: Valor = (HighByte * 256 + LowByte) / 1.2
    lux_val = ((buffHigh << 8) | buffLow) / 1.2;

    return lux_val;
}