/******************************************************************************************************
 * API_delay.c
 *
 *  Created on : Oct 9, 2026
 *  Author     : Plaate Ivan - Grupo 3 TDII 2026
 *****************************************************************************************************/

/* Includes ************************************************************************/
#include "main.h"
#include "API_delay.h"

/*Defines *************************************************************************/

/*Declaration of variables ********************************************************/

/*** Function definition ***********************************************************/
/**
  * @brief Inicializa un retardo no bloqueante.
  *        Carga la duracion e inicializa el flag running en false,
  *        sin iniciar el conteo.
  * @param delay: puntero a la estructura del retardo a inicializar
  * @param duration: duracion del retardo en milisegundos
  * @retval None
  */
void delayInit(delay_t * delay, tick_t duration)
{
	if (delay == NULL)
	{
		return;
	}
	delay->duration = duration;
	delay->running = false;
}

/**
  * @brief Consulta el estado del retardo.
  *        Si el retardo no esta corriendo, toma la marca de tiempo actual con
  *        HAL_GetTick() y arranca el conteo (devuelve false).
  *        Si esta corriendo, evalua si el tiempo transcurrido alcanzo la
  *        duracion: en ese caso reinicia el flag running y devuelve true.
  * @param delay: puntero a la estructura del retardo a consultar
  * @retval bool_t: true si el retardo cumplio su tiempo, false en caso contrario
  */
bool_t delayRead(delay_t * delay)
{
	bool_t ret = false;

	if (delay == NULL)
	{
		return ret;
	}

	if (!delay->running)
	{
		/* El retardo esta detenido: se toma la marca de tiempo y se inicia */
		delay->startTime = HAL_GetTick();
		delay->running = true;
	}
	else
	{
		/* El retardo esta corriendo: se verifica el tiempo transcurrido */
		if ((HAL_GetTick() - delay->startTime) >= delay->duration)
		{
			delay->running = false;
			ret = true;
		}
	}
	return ret;
}

/**
  * @brief Cambia la duracion de un retardo existente.
  * @param delay: puntero a la estructura del retardo a modificar
  * @param duration: nueva duracion del retardo en milisegundos
  * @retval None
  */
void delayWrite(delay_t * delay, tick_t duration)
{
	if (delay == NULL)
	{
		return;
	}
	delay->duration = duration;
}
