/*! @mainpage Guia2_Ej1
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
 * | 06/09/2024 | Document creation		                         |
 *
 * @author Manuela Calvo (manuela.calvo@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_PERIOD 1000
/*==================[internal data definition]===============================*/
TaskHandle_t mostrarDistancia_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
static void mostrarDistanciaTask(void *pvParameter){
    while(true){
        uint16_t distancia;
        distancia = HcSr04ReadDistanceInCentimeters();

        if(distancia < 10){
            LedsOffAll();
        }
        else if(distancia > 10 && distancia < 20){
            LedOn(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
        }
        else if(distancia > 20 && distancia < 30){
            LedOn(LED_1);
            LedOn(LED_2);
            LedOff(LED_3);
        }
        else if(distancia > 30){
            LedOn(LED_1);
            LedOn(LED_2);
            LedOn(LED_3);
        }

        LcdItsE0803Write(distancia);

        vTaskDelay(CONFIG_PERIOD / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
    HcSr04Init(GPIO_3, GPIO_2);
    LcdItsE0803Init();
    xTaskCreate(&mostrarDistanciaTask, "Mostrar distancia", 512, NULL, 5, &mostrarDistancia_task_handle);
}
/*==================[end of file]============================================*/