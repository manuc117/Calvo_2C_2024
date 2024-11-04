/*! @mainpage examen
 *
 * @section genDesc General Description
 *
 * Este programa permite medir, con un sensor de ultrasonido, la distancia a la que se encuentra un ciclista de un vehículo 
 * y advertirle al mismo mediante el encendido de leds, una alarma sonora y mensajes enviados a una aplicación de smartphone. 
 * También permite medir, con un acelerómetro, la aceleración del ciclista y advertirle por medio de la aplicación de smartphone
 * con un mensaje.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |   Peripheral   |    ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	 GPIO_3		|
 * | 	TRIGGER 	| 	 GPIO_2		|
 * |	BUZZER		|	 GPIO_20	|
 * |	  AD		|	   		|
 * | 	 +5V	 	| 	  +5V		|
 * | 	 GND	 	| 	   GND		|
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
#include "gpio_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
/**
 * @def CONFIG_PERIOD
 * @brief Periodo del temporizador en microsegundos para notificar la tarea de distancia.
 */
#define CONFIG_PERIOD_LEDS 500000 //Medir 2 veces por segundo, entonces medimos 1 vez cada 0.5 seg

/** 
 * @def CONFIG_PERIOD
 * @brief Período de configuración en microsegundos para el temporizador de la tarea del acelerómetro.
 */
#define CONFIG_PERIOD 100000

/**
 * @def CONFIG_PERIOD_BUZZER1
 * @brief 
 */
#define CONFIG_PERIOD_BUZZER1 1000

/**
 * @def CONFIG_PERIOD_BUZZER2
 * @brief 
 */
#define CONFIG_PERIOD_BUZZER2 500

/**
 * @brief Variable que almacena la distancia medida por el sensor de ultrasonido.
 */
uint16_t distancia;

/** 
 * @brief Variable para almacenar el valor de tensión leído en el eje X. 
 */
uint16_t ejeX;

/** 
 * @brief Variable para almacenar el valor de tensión leído en el eje Y. 
 */
uint16_t ejeY;

/** 
 * @brief Variable para almacenar el valor de tensión leído en el eje Z. 
 */
uint16_t ejeZ;

/** 
 * @brief Tension máxima en mV.
 */
uint16_t tensionMax = 2400;

/** 
 * @brief Variable para almacenar la suma escalar de las tensiones de los ejes X, Y y Z.
 */
uint16_t tension = 0;
/*==================[internal data definition]===============================*/
/**
 * @brief Handle para la tarea que mide la distancia y controla LEDs y la pantalla LCD.
 */
TaskHandle_t medirDistancia_task_handle = NULL;

/**
 * @brief Handle para la tarea 
 */
TaskHandle_t acelerometro_task_handle = NULL;

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
 * @fn void FuncTimerB(void* param)
 * 
 * @brief Función del temporizador que notifica a la tarea `acelerometroTask` para su ejecución.
 * 
 * @param param Parámetro opcional que se puede pasar a la función. No se utiliza en esta implementación.
 */
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR(acelerometro_task_handle, pdFALSE);    
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

			GPIOOn(GPIO_20);
			vTaskDelay(CONFIG_PERIOD_BUZZER1);
			GPIOOff(GPIO_20);

            UartSendString(UART_CONNECTOR, "Precaucion, vehiculo cerca.");
        }
        else if(distancia < 300){ //300 cm = 3 m
            LedOn(LED_1);;
            LedOn(LED_2);
            LedOn(LED_3);

			GPIOOn(GPIO_20);
			vTaskDelay(CONFIG_PERIOD_BUZZER2);
			GPIOOff(GPIO_20);

			UartSendString(UART_CONNECTOR, "Peligro, vehiculo cerca.");
        }
    }
}


/**
 * @fn static void acelerometroTask(void *pvParameter)
 * 
 * @brief Mide la distancia usando un sensor ultrasonido y controla los LEDs.
 * 
 * @param[in] pvParameter Parámetro opcional que se puede pasar a la tarea. No se utiliza en esta implementación.
 */
static void acelerometroTask (void *pvParameter){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	AnalogInputReadContinuous(CH1, &ejeX);
	AnalogInputReadContinuous(CH1, &ejeY);
	AnalogInputReadContinuous(CH1, &ejeZ);

	tension = ejeX + ejeY + ejeZ;

	if (tension > tensionMax){
		UartSendString(UART_CONNECTOR, "Caída detectada");
	} 

}

/*==================[external functions definition]==========================*/
void app_main(void){
	//Inicializo los leds.
    LedsInit();
    //Inicializo el sensor de ultrasonido.
    HcSr04Init(GPIO_3, GPIO_2);
	//Inicializo el GPIO
	GPIOInit(GPIO_20, GPIO_OUTPUT);

    //Defino la estructura para la configuración del timer.
    timer_config_t timer_medirDistancia = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD_LEDS,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

	timer_config_t timer_acelerometro = {
        .timer = TIMER_B,
        .period = CONFIG_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };

    //Inicializo el timer.
    TimerInit(&timer_medirDistancia);
	TimerInit(&timer_acelerometro);
    //Creación de la tarea "Mostrar distancia".
    xTaskCreate(&medirDistanciaTask, "Medir distancia", 512, NULL, 5, &medirDistancia_task_handle);
	xTaskCreate(&acelerometroTask, "Acelerometro", 512, NULL, 5, &acelerometro_task_handle);
    //Inicio el conteo del timer.
    TimerStart(timer_medirDistancia.timer);
	TimerStart(timer_acelerometro.timer);

	//Defino la configuración de la entrada analógica.
    analog_input_config_t entradaX = {
		.input = CH1,		
		.mode = ADC_CONTINUOUS,		
		.func_p = NULL,			
		.param_p = NULL,			
		.sample_frec = 100
	};

	analog_input_config_t entradaY = {
		.input = CH2,		
		.mode = ADC_CONTINUOUS,		
		.func_p = NULL,			
		.param_p = NULL,			
		.sample_frec = 100
	};

	analog_input_config_t entradaZ = {
		.input = CH3,		
		.mode = ADC_CONTINUOUS,		
		.func_p = NULL,			
		.param_p = NULL,			
		.sample_frec = 100
	};
    //Inicializo la entrada analógica.
	AnalogInputInit(&entradaX);
	AnalogInputInit(&entradaY);
	AnalogInputInit(&entradaZ);
    //Inicializo la salida analógica.
	AnalogOutputInit();

	//Defino la estructura para la configuración del puerto serie.
    serial_config_t configPuertoSerie = {
        .port = UART_CONNECTOR,	
	    .baud_rate = 9600,	
	    .func_p = NULL,			
	    .param_p = NULL
    };

	//Inicializo el puerto serie.
    UartInit(&configPuertoSerie);
    
}
/*==================[end of file]============================================*/