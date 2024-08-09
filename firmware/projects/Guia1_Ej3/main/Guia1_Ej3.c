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
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
uint8_t ON = 1;
uint8_t OFF = 2;
uint8_t TOGGLE = 3;

struct leds
{
    uint8_t mode;       /*ON, OFF, TOGGLE*/
	uint8_t n_led;        /*indica el número de led a controlar*/
	uint8_t n_ciclos;   /*indica la cantidad de ciclos de ncendido/apagado*/
	uint16_t periodo;    /*indica el tiempo de cada ciclo*/
} my_leds; 

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
//Función para encender, apagar y hacer parpadear un LED.
void LedsControl (struct leds* myLeds){
	
	//Encendido de un led.
	if (myLeds->mode == ON){
		switch(myLeds->n_led){
			case 1:
				LedOn(LED_1);
			break;
			case 2:
				LedOn(LED_2);
			break;
			case 3: 
				LedOn(LED_3);
			break;
		}
	}

	//Apagado de un led.
	else if (myLeds->mode == OFF){
		switch(myLeds->n_led){
			case 1:
				LedOff(LED_1);
			break;
			case 2:
				LedOff(LED_2);
			break;
			case 3: 
				LedOff(LED_3);
			break;
		}
	}

	//Parpadeo de un led.
	else if (myLeds->mode == TOGGLE){
		switch (myLeds->n_led){
			case 1:
				for(uint8_t i=0; i < myLeds->n_ciclos; i++){
					LedOn(LED_1);
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
					LedOff(LED_1);
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				}
			break;

			case 2:
				for(uint8_t i=0; i < myLeds->n_ciclos; i++){
					LedOn(LED_2);
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
					LedOff(LED_2);
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				}
			break;

			case 3:
				for(uint8_t i=0; i < myLeds->n_ciclos; i++){
					LedOn(LED_3);
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
					LedOff(LED_3);
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				}
			break;
		}
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	struct leds *ledsPtr; //Defino un puntero a la estructura.
	ledsPtr  = &my_leds;  //Asigno la dirección de memoria al puntero.
	ledsPtr->mode = TOGGLE;
	ledsPtr->n_led = 2;
	ledsPtr->n_ciclos = 5;

	LedsInit(); //Inicialización de los leds.
	LedsControl(ledsPtr); //LLamo a la función LedsControl.
}
/*==================[end of file]============================================*/