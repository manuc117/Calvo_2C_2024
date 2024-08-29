/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
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
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;


/*==================[internal functions declaration]=========================*/

//Función que cambia el estado de cada GPIO, a ‘0’ o a ‘1’, según el estado 
//del bit correspondiente en el BCD ingresado.
void estadoGPIO(gpioConf_t vec[4], uint8_t dato){
	for (uint8_t i = 0; i < 4; i++)
	{
		GPIOState(vec[i].pin, dato&1);
		dato = dato>>1;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t bcdNum = 8;
	gpioConf_t vecBits[4]; //Defino vector del tipo gpioConf_t.

	//Asigno el pin correspondiente a cada elemento del vector.
	vecBits[0].pin = GPIO_20;
	vecBits[1].pin = GPIO_21;
	vecBits[2].pin = GPIO_22;
	vecBits[3].pin = GPIO_23;

	//Asigno OUT a la dirección de cada GPIO.
	for (uint8_t i = 0; i<4; i++){
		vecBits[i].dir = 1;
	}

	estadoGPIO(vecBits, bcdNum); //Llamo a la función estadoGPIO
} 
/*==================[end of file]============================================*/