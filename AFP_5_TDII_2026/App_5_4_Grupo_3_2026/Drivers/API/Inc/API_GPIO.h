/*******************************************************************************************************
 * API_GPIO.h
 *
 *  Created on         : Oct 3, 2026
 *  Author             : Plaate Ivan - Grupo 3 TDII 2026
 *  Function of driver : Driver portable para manejo del modulo GPIO. Encapsula las funciones de la
 *  					 HAL de STM32 (HAL_GPIO_*) de forma que la aplicacion no dependa de la
 *  					 plataforma. Los pines se pasan por parametro mediante el tipo gpioPin_t,
 *  					 lo que permite reutilizar el driver en cualquier puerto, pin y placa.
 ******************************************************************************************************/

#ifndef API_INC_API_GPIO_H_
#define API_INC_API_GPIO_H_

/* Includes *******************************************************************************/
#include "stm32f4xx_hal.h"	/* Para poder usar GPIO_TypeDef y las funciones HAL_GPIO_* */
#include <stdint.h>			/* Para poder usar el tipo uint16_t en la definicion de gpioPin_t */
#include <stdbool.h>		/* Para poder definir el tipo buttonStatus_t que sera boolean */

/* Exported types *************************************************************************/
/* Estructura que identifica un pin fisico: puerto + mascara de pin. Es la clave de la
 * portabilidad del driver: las funciones reciben el pin como parametro y nada queda
 * hardcodeado a un puerto o placa en particular. */
typedef struct
{
	GPIO_TypeDef * port;	/* Puerto del pin (GPIOA..GPIOK). Usar los defines generados por CubeMX: xxx_GPIO_Port */
	uint16_t pin;			/* Mascara del pin (GPIO_PIN_x). Usar los defines generados por CubeMX: xxx_Pin */
} gpioPin_t;

typedef gpioPin_t led_t;		/* Un led es simplemente un pin de salida */
typedef gpioPin_t button_t;		/* Un pulsador es simplemente un pin de entrada */
typedef bool buttonStatus_t;	/* Estado del pulsador: true = presionado (activo), false = liberado */

/* Exported functions prototypes **********************************************************/
void MX_GPIO_Init(void);
void writeLedOn_GPIO(led_t led);
void writeLedOff_GPIO(led_t led);
void toggleLed_GPIO(led_t led);
buttonStatus_t readButton_GPIO(button_t button);

#endif /* API_INC_API_GPIO_H_ */
