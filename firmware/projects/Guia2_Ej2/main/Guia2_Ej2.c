/*! @mainpage Guia2_Ej2
 *
 * @section genDesc General Description
 *
 * Este código permite medir distancias mediante ultrasonido. Cuanto mayor es la distancia se van prendiendo leds verde,
 * amarillo y rojo. Además, se muestra la medición en un display y se puede mantener la última medición en pantalla tocando
 * una tecla.
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
/**
 * @def CONFIG_PERIOD_LEDS
 * @brief Periodo en milisegundos de retraso para el encendido de los LEDs.
 */
#define CONFIG_PERIOD_LEDS 1000

/**
 * @def CONFIG_PERIOD
 * @brief Periodo en milisegundos para la lectura de las teclas.
 */
#define CONFIG_PERIOD 1000000

/**
 * @brief Bandera para activar o desactivar el sistema.
 */
bool ACTIVAR = 1;

/**
 * @brief Bandera para pausar o permitir la actualización de la pantalla LCD.
 */
bool HOLD = 0;

/**
 * @brief Variable que almacena el estado de las teclas.
 */
uint8_t teclas;

/*==================[internal data definition]===============================*/
/**
 * @brief Handle para la tarea que muestra la distancia.
 */
TaskHandle_t mostrarDistancia_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/**
 * @fn void FuncTimer(void* param)
 * 
 * @brief Función del temporizador que notifica a la tarea `mostrarDistanciaTask` para su ejecución.
 * 
 * @param[in] param Parámetro opcional que se puede pasar a la función. No se utiliza en esta implementación.
 */
void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(mostrarDistancia_task_handle, pdFALSE);    
}

/**
 * @fn static void mostrarDistanciaTask(void *pvParameter)
 * 
 * @brief Mide la distancia usando un sensor ultrasonido y controla los LEDs y una pantalla LCD.
 * 
 * @param[in] pvParameter Parámetro opcional que se puede pasar a la tarea. No se utiliza en esta implementación.
 */
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

/**
 * @fn void activarSwitch()
 * 
 * @brief Alterna el estado de la variable `ACTIVAR` cuando se presiona el interruptor SWITCH_1.
 */
void activarSwitch(){
    ACTIVAR = !ACTIVAR;
}

/**
 * @fn void holdSwitch()
 * 
 * @brief Alterna el estado de la variable `HOLD` cuando se presiona el interruptor SWITCH_2.
 */
void holdSwitch(){
    HOLD = !HOLD;
}

/*==================[external functions definition]==========================*/
void app_main(void){
    //Inicializo los leds.
    LedsInit();
    //Incializo el sensor de ultrasonido.
    HcSr04Init(GPIO_3, GPIO_2);
    //Inicializo el display LCD.
    LcdItsE0803Init();
    //Inicializo los switches.
    SwitchesInit();

    //Defino la estructura para la configuración del timer.
    timer_config_t timer_mostrarDistancia = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD,
        .func_p = FuncTimer,
        .param_p = NULL
    };

    //Inicializo el timer.
    TimerInit(&timer_mostrarDistancia);
    //Creación de la tarea "Mostrar distancia".
    xTaskCreate(&mostrarDistanciaTask, "Mostrar distancia", 512, NULL, 5, &mostrarDistancia_task_handle);
    //Inicio el conteo del timer.
    TimerStart(timer_mostrarDistancia.timer);

    SwitchActivInt(SWITCH_1, &activarSwitch, NULL);
    SwitchActivInt(SWITCH_2, &holdSwitch, NULL);
}
/*==================[end of file]============================================*/