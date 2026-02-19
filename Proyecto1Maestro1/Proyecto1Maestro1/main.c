/*
 * Proyecto1Maestro1.c
 *
 * Created: 9/2/2026 23:53:10
 * Author : Jorge Cruz - 23502
 * Erick Perez - 23001
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>      // Necesaria para 'itoa' (convertir números a texto)
#include "I2C/I2C.h"
#include "LCD/LCD.h"
#include "BH1750.h"

// --- CONFIGURACIÓN SENSORES ---
#define MAX_LUX 2000     // Valor de lux que consideramos el 100% de luz

#define slaveUS1 0x30
#define slaveUS1R (0x30 << 1) | 0x01
#define slaveUS1W (0x30 << 1) & 0b11111110

#define slaveP2 0x32
#define slaveP2R (0x32 << 1) | 0x01
#define slaveP2W (0x32 << 1) & 0b11111110

uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C = 0;
uint8_t distancia = 0;


char buffer_distancia[10];
char bufferDisp[10];
uint16_t pasoFinal;      // Peso real calculado (descomprimido)
char bufferTexto[10];    // Buffer para texto del peso

uint8_t obtenerPorcentajeLuz(void);

int main(void)
{
	//DDRD |= (1<<DDD4);
	initLCD8bits();
    I2C_Master_Init(100000,1);
	BH1750_Init();
	LCD_Set_Cursor(1,1);
	LCD_Write_String("Dis: ");
	
	LCD_Set_Cursor(6,1);
	LCD_Write_String("Peso: ");
	
	LCD_Set_Cursor(12,1);
	LCD_Write_String("Luz: ");
	
    while (1) 
    {
		if(!I2C_Master_Start()) continue;;
		if (!I2C_Master_Write(slaveUS1W))
		{
			I2C_Master_Stop();
			continue;;
		}
		I2C_Master_Write('R');
		_delay_ms(5); 
		if (!I2C_Master_Repeated_Start())
		{
			I2C_Master_Stop();
			continue;
		}
		if (!I2C_Master_Write(slaveUS1R))
		{
			I2C_Master_Stop();
			continue;
		}
		I2C_Master_Read(&bufferI2C, 0);
		I2C_Master_Stop();
		distancia = bufferI2C;
		//Aqui empieza lo de interpretar info de los esclavos
		//COn esta distancia se si mi dc esta encendido o apagado, entonces aqui guardamos estado de motor y no hay que pedirselo al esclavo		
		LCD_Set_Cursor(1,2);
		LCD_Write_String("     ");
		LCD_Set_Cursor(1,2);
		itoa(distancia, buffer_distancia, 10);
		LCD_Write_String(buffer_distancia);
		_delay_ms(5);
		/*
		//Envar datos al esclavo 2 inicia aquí (inicialmente mandar distancia al servo)
		if(!I2C_Master_Start()) continue;

		if (!I2C_Master_Write(slaveP2W))   // dirección escritura servo
		{
			I2C_Master_Stop();
			continue;
		}

		// Enviar distancia a servo
		I2C_Master_Write(bufferI2C);

		I2C_Master_Stop();
		*/
		//
		
		//Mandar nivel de luz a esclavo ultrasónico
		
		
		
		
		//Leer esclavo peso
		//Por ahora comentarios en lo que conecto el sensor de peso y el otro esclavo
		//Solo falta lograr saber la informacion de donde esta el servo
		if(!I2C_Master_Start()) continue;
		if (!I2C_Master_Write(slaveP2W))
		{
			I2C_Master_Stop();
			continue;
		}
		I2C_Master_Write('R');
		_delay_ms(5);
		if (!I2C_Master_Repeated_Start())
		{
			I2C_Master_Stop();
			continue;
		}
		if (!I2C_Master_Write(slaveP2R))
		{
			I2C_Master_Stop();
			continue;
		}
		I2C_Master_Read(&bufferI2C, 0);
		I2C_Master_Stop();
		pasoFinal = (uint16_t)bufferI2C * 4;
		/*
		int8_t msb, lsb;
		I2C_Master_Read(&msb, 1);   // ACK para indicar que quieres más
		I2C_Master_Read(&lsb, 0);   // NACK en el último byte
		I2C_Master_Stop();

		pasoFinal = ((uint16_t)msb << 8) | lsb;
*/
		
		//Aqui empieza lo de interpretar info de los esclavos
		
		uint8_t porcentaje = obtenerPorcentajeLuz(); //igual con esto, volvemos a saber si el motor dc esta encendido o no por el nivel de luz
		
		itoa(porcentaje, bufferDisp, 10);
		
		if(!I2C_Master_Start()) continue;

		if (!I2C_Master_Write(slaveUS1W))   // dirección escritura servo
		{
			I2C_Master_Stop();
			continue;
		}

		I2C_Master_Write('L');
		I2C_Master_Write(porcentaje);
		I2C_Master_Stop();
		
				// Enviar distancia a servo
		

		
		LCD_Set_Cursor(6,2);
		LCD_Write_String("     ");
		LCD_Set_Cursor(6,2);
		itoa(pasoFinal, bufferTexto, 10);
		LCD_Write_String(bufferTexto);
		_delay_ms(5);
		
		LCD_Set_Cursor(11,2);
		LCD_Write_String("     ");
		LCD_Set_Cursor(11,2);
		itoa(porcentaje, bufferDisp, 10);
		LCD_Write_String(bufferDisp);
		
		_delay_ms(5);

    }
}

// Función para leer Lux y convertir a escala 0-100%
uint8_t obtenerPorcentajeLuz(void)
{
	// Leer valor crudo (0 a 65535)
	uint16_t lecturaRaw = BH1750_ReadLux();
	
	// Saturación: Si hay más luz que el máximo, limitar a 100%
	if (lecturaRaw >= MAX_LUX) return 100;

	// Regla de tres simple: (Lectura * 100) / Maximo
	// Usamos uint32_t para evitar desbordamiento matemático intermedio
	return (uint8_t)((uint32_t)lecturaRaw * 100 / MAX_LUX);
}
