/*! @mainpage examen
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
 * | 04/11/2024 | Document creation		                         |
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
#include "buzzer.h"
/*==================[macros and definitions]=================================*/
/**
 * @def CONFIG_PERIOD
 * @brief Periodo del temporizador en microsegundos para notificar la tarea de distancia.
 */
#define CONFIG_PERIOD_LEDS 500000 //Medir 2 veces por segundo, entonces medimos 1 vez cada 0.5 seg

/**
 * @brief Variable que almacena la distancia medida por el sensor de ultrasonido.
 */
uint16_t distancia;

/*==================[internal data definition]===============================*/
/**
 * @brief Handle para la tarea que mide la distancia y controla LEDs y la pantalla LCD.
 */
TaskHandle_t medirDistancia_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/**
 * @fn void FuncTimerA(void* param)
 * 
 * @brief Función del temporizador que notifica a la tarea `medirDistanciaTask` para su ejecución.
 * 
 * @param param Parámetro opcional que se puede pasar a la función. No se utiliza en esta implementación.
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(medirDistancia_task_handle, pdFALSE);    
}

/**
 * @fn static void medirDistanciaTask(void *pvParameter)
 * 
 * @brief Mide la distancia usando un sensor ultrasonido y controla los LEDs.
 * 
 * @param[in] pvParameter Parámetro opcional que se puede pasar a la tarea. No se utiliza en esta implementación.
 */
static void medirDistanciaTask(void *pvParameter){
    while (true){
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        distancia = HcSr04ReadDistanceInCentimeters();
        
        if(distancia > 500 ){ //500 cm = 5m
            LedOn(LED_1); //Led verde
            LedOff(LED_2); //Led amarillo
            LedOff(LED_3); //Led rojo
        }
        else if(distancia > 300 && distancia < 500){ //300 cm = 3 m y 500 cm = 5 m
            LedOn(LED_1);
            LedOn(LED_2);
            LedOff(LED_3);
        }
        else if(distancia < 300){ //300 cm = 3 m
            LedOn(LED_1);
            LedOn(LED_2);
            LedOn(LED_3);
        }

    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
	//Inicializo los leds.
    LedsInit();
    //Inicializo el sensor de ultrasonido.
    HcSr04Init(GPIO_3, GPIO_2);

    //Defino la estructura para la configuración del timer.
    timer_config_t timer_medirDistancia = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD_LEDS,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    //Inicializo el timer.
    TimerInit(&timer_medirDistancia);
    //Creación de la tarea "Mostrar distancia".
    xTaskCreate(&medirDistanciaTask, "Medir distancia", 512, NULL, 5, &medirDistancia_task_handle);
    //Inicio el conteo del timer.
    TimerStart(timer_medirDistancia.timer);

}
/*==================[end of file]============================================*/