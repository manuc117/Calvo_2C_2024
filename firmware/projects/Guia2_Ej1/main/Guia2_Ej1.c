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
 * |  EDU-CIAA-NXP  |   PERIFÉRICO  |
 * |:--------------:|:--------------|
 * | 	GPIO_3	 	| 	  ECHO	    |
 * | 	GPIO_2	 	| 	 TRIGGER	|
 * | 	 +5V	 	| 	  +5V		|
 * | 	 GND	 	| 	   GND		|
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
#include "switch.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_PERIOD_LEDS 1000
#define CONFIG_PERIOD_TECLAS 200
#define CONFIG_PERIOD 10
bool ACTIVAR = 1;
bool HOLD = 0;
uint8_t teclas;

/*==================[internal data definition]===============================*/
TaskHandle_t mostrarDistancia_task_handle = NULL;
TaskHandle_t leerTeclado_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
static void mostrarDistanciaTask(void *pvParameter){
    while (true){
        uint16_t distancia;
        distancia = HcSr04ReadDistanceInCentimeters();

        if(ACTIVAR == true){

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

            if(HOLD == false){   
                LcdItsE0803Write(distancia);
            } 
        }
        else if(ACTIVAR == false){
            LcdItsE0803Off();
            LedsOffAll();
        }

        vTaskDelay(CONFIG_PERIOD_LEDS / portTICK_PERIOD_MS);
    }
}

static void leerTecladoTask(void *pvParameter){
    while(true){
        teclas = SwitchesRead();
        switch (teclas){
            case SWITCH_1:
                ACTIVAR = !ACTIVAR;
                vTaskDelay(CONFIG_PERIOD_TECLAS / portTICK_PERIOD_MS);
            break;

            case SWITCH_2:
                HOLD = !HOLD;
                vTaskDelay(CONFIG_PERIOD_TECLAS / portTICK_PERIOD_MS);
            break;

            default:
                break;
        }

        vTaskDelay(CONFIG_PERIOD / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
    HcSr04Init(GPIO_3, GPIO_2);
    LcdItsE0803Init();
    SwitchesInit();
    xTaskCreate(&mostrarDistanciaTask, "Mostrar distancia", 512, NULL, 5, &mostrarDistancia_task_handle);
    xTaskCreate(&leerTecladoTask, "Leer teclado", 512, NULL, 5, &leerTeclado_task_handle);
}
/*==================[end of file]============================================*/