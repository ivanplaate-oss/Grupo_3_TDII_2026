/*******************************************************************************************************
 * API_debounce.h
 *
 *  Created on         : Oct 16, 2026
 *  Author             : Plaate Ivan - Grupo 3 TDII 2026
 *  Function of driver : Driver de antirrebote por software para pulsadores
 *  					 mecanicos, implementado como maquina de estados finitos
 *  					 (MEF) con validacion temporal de 40 ms basada en el
 *  					 driver API_delay. Es portable: la lectura del pulsador
 *  					 se inyecta como parametro, por lo que el driver no
 *  					 depende de pines, puertos ni de la placa utilizada.
 ******************************************************************************************************/

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

/* Includes *******************************************************************************/
#include "API_delay.h"		/* El driver usa delay_t y bool_t de API_delay */

/* Exported types *************************************************************************/
/* Estados de la maquina de antirrebote */
typedef enum
{
	BUTTON_UP,			/* Pulsador liberado (estado inicial) */
	BUTTON_FALLING,		/* Posible pulsacion: esperando validar 40 ms */
	BUTTON_DOWN,		/* Pulsador presionado (validado) */
	BUTTON_RISING,		/* Posible liberacion: esperando validar 40 ms */
} debounceState_t;

/* Exported functions prototypes **********************************************************/
void debounceFSM_init(void);
void debounceFSM_update(bool_t buttonRead);
bool_t readKey(void);

/* Acciones de flanco: se declaran en el driver pero las implementa cada
 * aplicacion (tipicamente con funciones de API_GPIO). Asi la MEF queda
 * portable y la accion asociada a presionar/soltar queda a cargo de la app. */
void buttonPressed(void);
void buttonReleased(void);

#endif /* API_INC_API_DEBOUNCE_H_ */
