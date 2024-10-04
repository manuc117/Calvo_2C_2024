/*! @mainpage Guia2_Ej4
 *
 * @section genDesc General Description
 *
 * Este programa permite convertir entradas analógicas en salidas digitales y, además, permite convertir una señal digital de  
 * ecg en una señal analógica.
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
 * | 27/09/2024 | Document creation		                         |
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
#include "lcditse0803.h"
#include "switch.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
/** 
 * @def CONFIG_PERIOD
 * @brief Período de configuración en microsegundos para el temporizador de la tarea que convierte datos analógicos en digitales. 
 */
#define CONFIG_PERIOD 2000

/**
 * @def CONFIG_PERIOD_ECG 
 * @brief Período de configuración en microsegundos para el temporizador de la tarea que permite escribir el ECG.
 */
#define CONFIG_PERIOD_ECG 4329

/**
 * @def BUFFER_SIZE
 * @brief Tamaño del buffer de ECG.
 */
#define BUFFER_SIZE 231


/** 
 * @brief Variable para almacenar el valor de tensión leído. 
 */
uint16_t tension;

/**
 * @brief Contador para el buffer de datos de ECG.
 */
uint8_t contadorEcg = 0;
/*==================[internal data definition]===============================*/
/**
 * @brief Handle para la tarea de conversión analógico-digital.
 */
TaskHandle_t convertirAD_task_handle = NULL;

/**
 * @brief Handle para la tarea que convierte una señal digital de ecg en una analógica.
 */
TaskHandle_t escribirEcg_task_handle = NULL;

/**
 * @brief Buffer de datos de la señal digital de ecg.
 */
const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};
/*==================[internal functions declaration]=========================*/
/**
 * @fn void FuncTimerAD(void* param)
 * 
 * @brief Función del temporizador que notifica a la tarea `convertirADTask` para su ejecución.
 * 
 * @param param Parámetro opcional que se puede pasar a la función. No se utiliza en esta implementación.
 */
void FuncTimerAD(void* param){
    vTaskNotifyGiveFromISR(convertirAD_task_handle, pdFALSE);    
}

/**
 * @fn void FuncTimerEcg(void* param)
 * 
 * @brief Función del temporizador que notifica a la tarea `escribirEcgTask` para su ejecución.
 * 
 * @param param Parámetro opcional que se puede pasar a la función. No se utiliza en esta implementación.
 */
void FuncTimerEcg(void* param){
	vTaskNotifyGiveFromISR(escribirEcg_task_handle, pdFALSE);
}

/**
 * @fn static void convertirADTask(void *pvParameter)
 * 
 * @brief Función que permite convertir datos analógicos en digitales. Lee la entrada analógica desde el canal 
 * especificado y envía los datos a través de UART.
 * 
 * @param[in] pvParameter Parámetro opcional que se puede pasar a la tarea. No se utiliza en esta implementación.
 */
static void convertirADTask(void *pvParameter){
	while (true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1, &tension);
		UartSendString(UART_PC, (const char*)UartItoa(tension, 10));
		UartSendString(UART_PC, " \r\n");
	}
}

/**
 * @fn static void escribirEcgTask(void* pvParameter)
 * 
 * @brief Función que permite convertir una señal digital de ecg en una señal analógica. 
 * 
 * @param[in] pvParameter Parámetro opcional que se puede pasar a la tarea. No se utiliza en esta implementación.
 */
static void escribirEcgTask(void *pvParameter){
	while (true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogOutputWrite(ecg[contadorEcg]);
		
		if(contadorEcg < BUFFER_SIZE-2){
			contadorEcg++;
		}
		else {
			contadorEcg = 0;
		}
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	//Defino la configuración de la entrada analógica.
    analog_input_config_t entradaAnalog = {
		.input = CH1,		
		.mode = ADC_SINGLE,		
		.func_p = NULL,			
		.param_p = NULL,			
		.sample_frec = NULL
	};

    //Inicializo la entrada analógica.
	AnalogInputInit(&entradaAnalog);
    //Inicializo la salida analógica.
	AnalogOutputInit();

    //Defino la configuración del timer para la tarea "Convertir AD".
	timer_config_t configTimerAD = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD,
        .func_p = FuncTimerAD,
        .param_p = NULL
    };

    //Defino la configuración del timer para la tarea "Escribir ECG".
	timer_config_t configTimerEcg= {
        .timer = TIMER_B,
        .period = CONFIG_PERIOD_ECG,
        .func_p = FuncTimerEcg,
        .param_p = NULL
    };
	
	//Inicializo el timers.
    TimerInit(&configTimerAD);
	TimerInit(&configTimerEcg);
	//Creación de la tareas.
    xTaskCreate(&convertirADTask, "Convertir AD", 2048, NULL, 5, &convertirAD_task_handle);
	xTaskCreate(&escribirEcgTask, "Escribir ECG", 2048, NULL, 5, &escribirEcg_task_handle);
    //Inicio el conteo de los timers.
    TimerStart(configTimerAD.timer);
	TimerStart(configTimerEcg.timer);

	//Defino la estructura para la configuración del puerto serie.
    serial_config_t configPuertoSerie = {
        .port = UART_PC,	
	    .baud_rate = 115200,	
	    .func_p = NULL,			
	    .param_p = NULL
    };
    //Inicializo el puerto serie.
    UartInit(&configPuertoSerie);


}
/*==================[end of file]============================================*/