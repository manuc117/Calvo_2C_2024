/*! @mainpage Guia2_Ej4
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
#define CONFIG_PERIOD 2000
uint16_t tension;
/*==================[internal data definition]===============================*/
TaskHandle_t convertirAD_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(convertirAD_task_handle, pdFALSE);    
}

void convertirADTask(void *pvParameter){
	while (true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1, &tension);
		UartSendString(UART_PC, (const char*)UartItoa(tension, 10));
		UartSendString(UART_PC, " \r\n");
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	analog_input_config_t entradaAnalog = {
		.input = CH1,		
		.mode = ADC_SINGLE,		
		.func_p = NULL,			
		.param_p = NULL,			
		.sample_frec = NULL
	};

	//Inicializo la entrada analógica
	AnalogInputInit(&entradaAnalog);

	timer_config_t configTimer = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD,
        .func_p = FuncTimer,
        .param_p = NULL
    };

	//Inicializo el timer.
    TimerInit(&configTimer);
	//Creación de la tarea "Mostrar distancia".
    xTaskCreate(&convertirADTask, "Convertir AD", 2048, NULL, 5, &convertirAD_task_handle);
    //Inicio el conteo del timer.
    TimerStart(configTimer.timer);

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