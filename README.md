# Grupo 3 - Tecnicas Digitales II - 2026

Repositorio grupal de la asignatura Tecnicas Digitales II (UTN-FRT, Ing. Electronica).

Placa utilizada: **STM32 NUCLEO-F429ZI** (STM32F429ZITx, ARM Cortex-M4F).
Entorno: **STM32CubeIDE 2.1.1** + STM32CubeMX 6.17 (solo HAL).

## Estructura del repositorio

Cada Actividad de Formacion Practica tiene su carpeta con las aplicaciones desarrolladas:

```
Grupo_3_TDII_2026/
├── AFP_3_TDII_2026/
│   ├── App_3_1_Grupo_3_2026/
│   ├── App_3_2_Grupo_3_2026/
│   ├── App_3_3_Grupo_3_2026/
│   └── App_3_4_Grupo_3_2026/
├── AFP_4_TDII_2026/
│   ├── App_4_1_Grupo_3_2026/
│   ├── App_4_2_Grupo_3_2026/
│   ├── App_4_3_Grupo_3_2026/
│   └── App_4_4_Grupo_3_2026/
└── AFP_5_TDII_2026/
    ├── App_5_1_Grupo_3_2026/
    ├── App_5_2_Grupo_3_2026/
    ├── App_5_3_Grupo_3_2026/
    └── App_5_4_Grupo_3_2026/
```

## AFP 3 - Driver GPIO

Creacion del driver `API_GPIO` (en `Drivers/API/Inc` y `Drivers/API/Src`) e
integracion en las aplicaciones de secuencias de LEDs desarrolladas en la AFP1,
reemplazando las llamadas directas a la HAL por funciones del driver:

| App | Refactor de | Descripcion |
|-----|-------------|-------------|
| App_3_1 | App 1.1 | Secuencia LD1->LD2->LD3, 200 ms encendido / 200 ms apagado |
| App_3_2 | App 1.2 | Idem + el pulsador invierte el sentido de la secuencia |
| App_3_3 | App 1.3 | Pulsador alterna 4 secuencias (150 ms secuencial, parpadeo simultaneo 300 ms, periodos independientes 100/300/600 ms, LD1+LD3 vs LD2 inverso 150 ms) |
| App_3_4 | App 1.4 | Los 3 leds parpadean juntos; el pulsador cicla el tiempo de alternancia: 100 / 250 / 500 / 1000 ms |

El driver es **portable**: los pines se identifican mediante la estructura
`gpioPin_t {puerto, pin}` pasada por parametro, por lo que no queda atado a un
puerto ni a una placa en particular. Funciones exportadas:
`writeLedOn_GPIO`, `writeLedOff_GPIO`, `toggleLed_GPIO`, `readButton_GPIO` y
`MX_GPIO_Init` (la inicializacion de pines vive dentro del driver).

## AFP 4 - Retardos no bloqueantes

Creacion del driver `API_delay` (`delay_t` basado en `HAL_GetTick`) e
integracion en las mismas aplicaciones, reemplazando `HAL_Delay()` por
retardos no bloqueantes. El lazo principal nunca se detiene: el pulsador se
lee en cada iteracion y responde de inmediato.

Funciones exportadas: `delayInit`, `delayRead`, `delayWrite`. Tipos: `tick_t`,
`bool_t`, `delay_t` (cada instancia es un retardo independiente — en App_4_3
se usan 3 retardos simultaneos, uno por led).

Los drivers se acumulan: cada proyecto de AFP4 carga `API_GPIO` y `API_delay`.

## AFP 5 - Antirrebote por maquina de estados

Creacion del driver `API_debounce`: una maquina de estados finitos de cuatro
estados (`BUTTON_UP`, `BUTTON_FALLING`, `BUTTON_DOWN`, `BUTTON_RISING`) que
valida cada flanco del pulsador con un retardo de 40 ms provisto por
`API_delay`, eliminando el rebote mecanico por software.

El driver es **independiente del hardware**: no lee ningun pin ni acciona
leds. La aplicacion le inyecta la lectura logica del boton
(`debounceFSM_update(readButton_GPIO(userButton))`) y define las acciones de
cada flanco implementando las funciones `buttonPressed()` y
`buttonReleased()`, declaradas en el driver.

Funciones exportadas: `debounceFSM_init`, `debounceFSM_update`, `readKey`
(evento one-shot de pulsacion). Las cuatro aplicaciones repiten los
comportamientos de AFP4, ahora con el pulsador filtrado por la MEF:

| App | Descripcion |
|-----|-------------|
| App_5_1 | Validacion del antirrebote: pulsacion -> invierte LD1, liberacion -> invierte LD3 |
| App_5_2 | Secuencia LD1->LD2->LD3; cada pulsacion validada invierte el sentido |
| App_5_3 | Pulsacion validada alterna las 4 secuencias |
| App_5_4 | Pulsacion validada cicla el tiempo de alternancia: 100 / 250 / 500 / 1000 ms |

Los drivers se acumulan: cada proyecto de AFP5 carga `API_GPIO`, `API_delay`
y `API_debounce`.
