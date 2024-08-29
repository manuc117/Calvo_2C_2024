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
 * | 09/08/2024 | Document creation		                         |
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

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/


/*==================[internal functions declaration]=========================*/

//Función que recibe un dato de 32 bits y lo convierte a BCD guardando cada dígito
//en el arreglo pasado como puntero.
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for(uint8_t i=0; i < digits; i++){
		bcd_number[digits-1-i] = data % 10;
		data = data/10;
	}

	return 0;
}

/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t valor;
	uint8_t digitos = 3;
	uint8_t bcd[digitos]; //Defino el arreglo.
	
	valor = 117;

	convertToBcdArray(valor, digitos, bcd); //LLamo a la función convertToBcdArray
	
	//Muestro los dígitos del dato por consola.
	for(uint8_t n=0; n < digitos; n++){
		printf("%u",bcd[n]);
	}
}	
/*==================[end of file]============================================*/