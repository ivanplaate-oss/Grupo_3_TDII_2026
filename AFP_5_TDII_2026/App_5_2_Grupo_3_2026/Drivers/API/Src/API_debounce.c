/******************************************************************************************************
 * API_debounce.c
 *
 *  Created on : Oct 16, 2026
 *  Author     : Plaate Ivan - Grupo 3 TDII 2026
 *****************************************************************************************************/

/* Includes ************************************************************************/
#include "API_debounce.h"
#include "API_delay.h"

/*Defines *************************************************************************/
#define DEBOUNCE_DELAY	40		/* Tiempo de validacion del antirrebote [ms] */

/*Declaration of variables ********************************************************/
static debounceState_t actualState;	/* Estado actual de la MEF */
static bool_t keyPressed;			/* Marca de evento de pulsacion validada */
static delay_t debounceDelay;		/* Retardo no bloqueante de 40 ms */

/*** Function definition ***********************************************************/
/**
  * @brief Inicializa la maquina de estados de antirrebote.
  *        Debe llamarse una vez antes de usar debounceFSM_update().
  * @retval None
  */
void debounceFSM_init(void)
{
	actualState = BUTTON_UP;
	keyPressed = false;
	delayInit(&debounceDelay, DEBOUNCE_DELAY);
}

/**
  * @brief Actualiza la maquina de estados de antirrebote.
  *        Debe llamarse periodicamente (en cada iteracion del lazo principal).
  *        La lectura logica del pulsador se inyecta como parametro:
  *        true = pulsador presionado, false = pulsador liberado.
  * @param buttonRead: lectura actual del pulsador (p. ej. readButton_GPIO())
  * @retval None
  */
void debounceFSM_update(bool_t buttonRead)
{
	switch (actualState)
	{
	case BUTTON_UP:
		if (buttonRead)
		{
			actualState = BUTTON_FALLING;	/* Posible pulsacion: arranca validacion */
			delayRead(&debounceDelay);
		}
		break;

	case BUTTON_FALLING:
		if (delayRead(&debounceDelay))		/* Pasaron los 40 ms */
		{
			if (buttonRead)
			{
				buttonPressed();			/* Pulsacion validada: accion de la app */
				keyPressed = true;
				actualState = BUTTON_DOWN;
			}
			else
			{
				actualState = BUTTON_UP;	/* Era rebote: descartada */
			}
		}
		break;

	case BUTTON_DOWN:
		if (!buttonRead)
		{
			actualState = BUTTON_RISING;	/* Posible liberacion: arranca validacion */
			delayRead(&debounceDelay);
		}
		break;

	case BUTTON_RISING:
		if (delayRead(&debounceDelay))		/* Pasaron los 40 ms */
		{
			if (!buttonRead)
			{
				buttonReleased();			/* Liberacion validada: accion de la app */
				actualState = BUTTON_UP;
			}
			else
			{
				actualState = BUTTON_DOWN;	/* Era rebote: sigue presionado */
			}
		}
		break;

	default:
		debounceFSM_init();					/* Estado invalido: reiniciar la MEF */
		break;
	}
}

/**
  * @brief Informa si hubo una pulsacion validada por la MEF (flanco descendente).
  *        La marca se reinicia al ser leida: devuelve true una sola vez por
  *        cada pulsacion validada.
  * @retval bool_t: true si se detecto una pulsacion desde la ultima lectura
  */
bool_t readKey(void)
{
	bool_t ret = false;
	if (keyPressed)
	{
		keyPressed = false;
		ret = true;
	}
	return ret;
}
