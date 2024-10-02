/*! @mainpage Guia2_Ej1
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
/**
 * @def CONFIG_PERIOD_LEDS
 * @brief Periodo en milisegundos de retraso para el encendido de los LEDs.
 */
#define CONFIG_PERIOD_LEDS 1000

/**
 * @def CONFIG_PERIOD_TECLAS
 * @brief Periodo en milisegundos para el cambio de estado de HOLD y ACTIVAR.
 */
#define CONFIG_PERIOD_TECLAS 200

/**
 * @def CONFIG_PERIOD
 * @brief Periodo en milisegundos para la lectura de las teclas.
 */
#define CONFIG_PERIOD 10

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

/**
 * @brief Handle para la tarea que lee el teclado.
 */
TaskHandle_t leerTeclado_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

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
        distancia = HcSr04ReadDistanceInCentimeters(); /**< Lee la distancia en centímetros del sensor HC-SR04. */

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

        vTaskDelay(CONFIG_PERIOD_LEDS / portTICK_PERIOD_MS);    /**< Espera un período antes de la siguiente medición. */ 
    }
}

/**
 * @fn static void leerTecladoTask(void *pvParameter)
 * 
 * @brief Lee el estado de las teclas y permite activar o desactivar los leds y el display, y mantener o no la medición en el display.
 * 
 * @param[in] pvParameter Parámetro opcional que se puede pasar a la tarea. No se utiliza en esta implementación.
 */
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
    //Inicializo los leds.
	LedsInit(); 
    //Inicializo el sensor de ultrasonido.
    HcSr04Init(GPIO_3, GPIO_2);
    //Inicializo el display LCD.
    LcdItsE0803Init();
    //Inicializo los switches.
    SwitchesInit();

    //Creación de tareas.
    xTaskCreate(&mostrarDistanciaTask, "Mostrar distancia", 512, NULL, 5, &mostrarDistancia_task_handle);
    xTaskCreate(&leerTecladoTask, "Leer teclado", 512, NULL, 5, &leerTeclado_task_handle);
}
/*==================[end of file]============================================*/