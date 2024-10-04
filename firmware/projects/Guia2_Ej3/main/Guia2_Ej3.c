/*! @mainpage Guia2_Ej3
 *
 * @section genDesc General Description
 *
 * Este código permite medir distancias mediante ultrasonido. Cuanto mayor es la distancia se van prendiendo leds verde,
 * amarillo y rojo. Además, se muestra la medición en un display y se puede mantener la última medición en pantalla tocando
 * una tecla. Por otro lado, el programa permite realizar las acciones de los switch con teclas del teclado de la PC con 
 * comunicación UART.
 *
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
 * | 20/09/2024 | Document creation		                         |
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
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/
/**
 * @def CONFIG_PERIOD
 * @brief Periodo del temporizador en microsegundos para notificar la tarea de distancia.
 */
#define CONFIG_PERIOD 1000000

/**
 * @brief Bandera para activar o desactivar el sistema.
 */

bool ACTIVAR = 0;

/**
 * @brief Bandera para pausar o permitir la actualización de la pantalla LCD.
 */
bool HOLD = 0;

/**
 * @brief Variable que almacena el estado de las teclas.
 */
uint8_t teclas;

/**
 * @brief Variable que almacena la distancia medida por el sensor de ultrasonido.
 */
uint16_t distancia;
        
/*==================[internal data definition]===============================*/
/**
 * @brief Handle para la tarea que mide la distancia y controla LEDs y la pantalla LCD.
 */
TaskHandle_t mostrarDistancia_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/**
 * @fn void FuncTimer(void* param)
 * 
 * @brief Función del temporizador que notifica a la tarea `mostrarDistanciaTask` para su ejecución.
 * 
 * @param param Parámetro opcional que se puede pasar a la función. No se utiliza en esta implementación.
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

            UartSendString(UART_PC, (const char*)UartItoa(distancia, 10));
            UartSendString(UART_PC, " cm\r\n");
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

/**
 * @fn void atenderTecladoPC()
 * 
 * @brief Maneja los comandos enviados por UART desde el PC para controlar el sistema. Recibe los caracteres enviados 
 * desde el puerto serie (UART) y ejecuta acciones basadas en ellos.
 */
void atenderTecladoPC(){
    uint8_t teclasPC;
    UartReadByte(UART_PC, &teclasPC);
    
    switch(teclasPC){
        case 'O':
            activarSwitch(); 
        break;

        case 'H':
            holdSwitch(); 
        break;

        default:
            break;
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
    //Incializo los switches.
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

    //Defino la estructura para la configuración del puerto serie.
    serial_config_t configPuertoSerie = {
        .port = UART_PC,	
	    .baud_rate = 9600,	
	    .func_p = atenderTecladoPC,			
	    .param_p = NULL
    };

    //Inicializo el puerto serie.
    UartInit(&configPuertoSerie);
    
}
/*==================[end of file]============================================*/