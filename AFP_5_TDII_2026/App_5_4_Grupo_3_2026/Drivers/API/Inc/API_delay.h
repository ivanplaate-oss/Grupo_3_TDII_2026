/*******************************************************************************************************
 * API_delay.h
 *
 *  Created on         : Oct 9, 2026
 *  Author             : Plaate Ivan - Grupo 3 TDII 2026
 *  Function of driver : Driver de retardos no bloqueantes basado en el tick de
 *  					 sistema (HAL_GetTick). Permite gestionar multiples
 *  					 retardos simultaneos e independientes sin detener la
 *  					 ejecucion del programa, reemplazando a HAL_Delay().
 *  					 Es totalmente portable: no depende de pines, puertos
 *  					 ni de la placa utilizada.
 ******************************************************************************************************/

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

/* Includes *******************************************************************************/
#include <stdint.h>			/* Para poder usar el tipo uint32_t en tick_t */
#include <stdbool.h>		/* Para poder usar el tipo bool en bool_t */

/* Exported types *************************************************************************/
typedef uint32_t tick_t;	/* Tipo para tiempos expresados en ticks de sistema (1 tick = 1 ms) */
typedef bool bool_t;		/* Tipo booleano para el flag de estado del retardo */

/* Estructura que contiene el contexto de un retardo no bloqueante.
 * Cada delay_t es independiente: permite tantos retardos simultaneos como
 * instancias se declaren. */
typedef struct
{
	tick_t startTime;	/* Marca de tiempo en la que inicio el conteo [ms] */
	tick_t duration;	/* Duracion del retardo [ms] */
	bool_t running;		/* Estado: true = contando, false = detenido */
} delay_t;

/* Exported functions prototypes **********************************************************/
void delayInit(delay_t * delay, tick_t duration);
bool_t delayRead(delay_t * delay);
void delayWrite(delay_t * delay, tick_t duration);

#endif /* API_INC_API_DELAY_H_ */
