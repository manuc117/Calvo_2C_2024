/*! @mainpage Guia1_Ej6
 *
 * @section genDesc General Description
 *
 * Este código permite mostrar por display un dato de 32 bits que recibe.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    EDU-ESP     |  Periférico   |
 * |:--------------:|:--------------|
 * | 	GPIO_20	 	| 		D1		|
 * | 	GPIO_21	 	| 		D2		|
 * | 	GPIO_22	 	| 		D3		|
 * | 	GPIO_23		| 		D4		|
 * | 	GPIO_19		| 	   SEL_1	|
 * | 	GPIO_18		| 	   SEL_2	|
 * | 	GPIO_9		| 	   SEL_3	|
 * | 	 +5V		| 	   +5V		|
 * | 	 GND		| 	   GND		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 16/08/2024 | Document creation		                         |
 *
 * @author Manuela Calvo (manuela.calvo@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
/**
 * @struct gpioConf_t
 * 
 * @brief Información de los pines.
 */
typedef struct
{
	gpio_t pin;			/*!< Número de los pines GPIO*/
	io_t dir;			/*!< Dirección del GPIO '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
/**
 * @fn int8_t convertToBcdArray(uint32_t datam, uint8_t digits, uint8_t *bcd_number)
 * 
 * @brief Convierte el dato recibido a BCD.
 * 
 * @param[in] data dato que es convertido a BCD.
 * @param[in] digits cantidad de digitos del dato.
 * @param[out] bcd_number puntero a un arreglo donde se cargan los dígitos del dato.
 * 
 * @return Retorna 0 si la conversión fue exitosa.
 */
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for(uint8_t i=0; i < digits; i++){
		bcd_number[digits-1-i] = data % 10;
		data = data/10;
	}

	return 0;
}

/**
 * @fn void estadoGPIO(gpioConf_t vec[4], uint8_t datoBcd)
 * 
 * @brief Cambia el estado de cada GPIO, a ‘0’ o a ‘1’, según el estado del bit correspondiente en el BCD ingresado. 
 * 
 * @param[in] vec  Vector de estructuras 'gpioConf_t' que mapea los bits del dato ingresado con los puertos.
 * @param[in] datoBcd Valor en BCD.
 */
void estadoGPIO(gpioConf_t vec[4], uint8_t datoBcd){
	for (uint8_t i = 0; i < 4; i++)
	{
		GPIOState(vec[i].pin, datoBcd&1);
		datoBcd = datoBcd>>1;
	}
}

/**
 * @fn void mostrarEnDisplay(gpioConf_t vecA[4], gpioConf_t vecB[3], uint32_t dato, uint8_t digitos)
 * 
 * @brief Muestra por display el dato que recibe.
 * 
 * @param[in] vecA Vector de estructuras 'gpioConf_t' que mapea los bits del dato ingresado con los puertos.
 * @param[in] vecB Vector de estructuras  'gpioConf_t' para seleccionar el dígito activo en el display LCD.
 * @param[in] dato Dato a ser mostrado por display.
 * @param[in] digitos Cantidad de digitos del dato.
 */
void mostrarEnDisplay(gpioConf_t vecA[4], gpioConf_t vecB[3], uint32_t dato, uint8_t digitos){
	uint8_t bcd[digitos];
	convertToBcdArray(dato, digitos, bcd);

	for (size_t i = 0; i < digitos; i++)
	{
		estadoGPIO(vecA, bcd[i]);
		GPIOOn(vecB[i].pin);
		GPIOOff(vecB[i].pin);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	gpioConf_t vecBits[4];
	gpioConf_t lcdVec[3];

	vecBits[0].pin = GPIO_20;
	vecBits[1].pin = GPIO_21;
	vecBits[2].pin = GPIO_22;
	vecBits[3].pin = GPIO_23;

	lcdVec[0].pin = GPIO_19;
	lcdVec[1].pin = GPIO_18;
	lcdVec[2].pin = GPIO_9;

	for (uint8_t i = 0; i < 4; i++){
		vecBits[i].dir = GPIO_OUTPUT;
		GPIOInit(vecBits[i].pin, vecBits[i].dir);
	}

	for (uint8_t i = 0; i < 3; i++){
		lcdVec[i].dir = GPIO_OUTPUT;
		GPIOInit(lcdVec[i].pin, lcdVec[i].dir);
	}
	
	uint32_t valor = 456;
	uint8_t digitos = 3;

	mostrarEnDisplay(vecBits, lcdVec, valor, digitos);
}
/*==================[end of file]============================================*/