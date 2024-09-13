/*! @mainpage Guia2_Ej2
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
 * | 13/09/2024 | Document creation		                         |
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
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_PERIOD_LEDS 1000
#define CONFIG_PERIOD 1000000
bool ACTIVAR = 1;
bool HOLD = 0;
uint8_t teclas;

/*==================[internal data definition]===============================*/
TaskHandle_t mostrarDistancia_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(mostrarDistancia_task_handle, pdFALSE);    
}

static void mostrarDistanciaTask(void *pvParameter){
    while (true){
        uint16_t distancia;
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ACTIVAR == true){

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

            if(HOLD == false){   
                LcdItsE0803Write(distancia);
            } 
        }
        else if(ACTIVAR == false){
            LcdItsE0803Off();
            LedsOffAll();
        }
    }
}

void activarSwitch(){
    ACTIVAR = !ACTIVAR;
}

void holdSwitch(){
    HOLD = !HOLD;
}

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
    HcSr04Init(GPIO_3, GPIO_2);
    LcdItsE0803Init();
    SwitchesInit();

    timer_config_t timer_mostrarDistancia = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD,
        .func_p = FuncTimer,
        .param_p = NULL
    };
    TimerInit(&timer_mostrarDistancia);
    xTaskCreate(&mostrarDistanciaTask, "Mostrar distancia", 512, NULL, 5, &mostrarDistancia_task_handle);
    TimerStart(timer_mostrarDistancia.timer);

    SwitchActivInt(SWITCH_1, &activarSwitch, NULL);
    SwitchActivInt(SWITCH_2, &holdSwitch, NULL);
}
/*==================[end of file]============================================*/