/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * App_4_3_Grupo_3_2026 - AFP4 - Retardos no bloqueantes (API_delay)
  *
  * Aplicacion (refactor de App_3_3 a no bloqueante): el pulsador onboard
  * alterna entre cuatro secuencias distintas de los 3 leds onboard:
  *   Secuencia 1: encendido secuencial LD1->LD2->LD3, alternancia de 150 ms.
  *   Secuencia 2: los tres leds parpadean simultaneamente, alternancia 300 ms.
  *   Secuencia 3: LD1 parpadea a 100 ms, LD2 a 300 ms y LD3 a 600 ms
  *                (un retardo no bloqueante independiente por led).
  *   Secuencia 4: LD1 y LD3 parpadean juntos y LD2 en forma inversa, 150 ms.
  * Cada pulsacion avanza a la siguiente secuencia (1->2->3->4->1...).
  * Se reemplaza HAL_Delay() por el driver de retardos no bloqueantes API_delay.
  * La gestion del modulo GPIO se realiza exclusivamente con el driver API_GPIO.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "API_GPIO.h"
#include "API_delay.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_COUNT	3		/* Cantidad de leds del vector */
#define SEQ_COUNT	4		/* Cantidad de secuencias disponibles */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ETH_TxPacketConfig TxConfig;
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
/* Vector de leds de la placa: con este formato la app queda general y puede
 * extenderse a mas leds con minimas modificaciones (solo agregar elementos) */
led_t leds[LED_COUNT] = {
	{LD1_GPIO_Port, LD1_Pin},
	{LD2_GPIO_Port, LD2_Pin},
	{LD3_GPIO_Port, LD3_Pin}
};

const button_t userButton = {USER_Btn_GPIO_Port, USER_Btn_Pin};

/* Tiempos de alternancia de cada secuencia [ms].
 * La secuencia 3 no usa este vector: tiene un retardo independiente por led. */
const tick_t tiempoSecuencia[SEQ_COUNT] = {150, 300, 0, 150};

/* Periodos independientes de cada led para la secuencia 3 [ms] */
const tick_t periodoLed[LED_COUNT] = {100, 300, 600};

delay_t delaySecuencia;				/* Retardo comun de las secuencias 1, 2 y 4 */
delay_t delayLed[LED_COUNT];			/* Un retardo por led para la secuencia 3 */

buttonStatus_t botonAnterior = false;	/* Estado anterior del pulsador (para flanco) */
uint8_t secuencia = 0;					/* Secuencia activa: 0..SEQ_COUNT-1 */
uint8_t indice = 0;						/* Indice del led en la secuencia 1 */
bool ledEncendido = false;				/* Estado de fase (secuencias 1 y 4) */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* El prototipo de MX_GPIO_Init() paso al driver: ver API_GPIO.h */
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */
static void reiniciarEstado(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
  reiniciarEstado();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* Lectura del pulsador en cada iteracion: respuesta inmediata */
	  buttonStatus_t actual = readButton_GPIO(userButton);
	  if (actual && !botonAnterior)
	  {
		  secuencia = (secuencia + 1) % SEQ_COUNT;
		  reiniciarEstado();	/* La nueva secuencia arranca desde estado conocido */
	  }
	  botonAnterior = actual;

	  switch (secuencia)
	  {
	  case 0:	/* Secuencia 1: encendido secuencial, 150 ms por led */
		  if (delayRead(&delaySecuencia))
		  {
			  if (!ledEncendido)
			  {
				  writeLedOn_GPIO(leds[indice]);
				  ledEncendido = true;
			  }
			  else
			  {
				  writeLedOff_GPIO(leds[indice]);
				  ledEncendido = false;
				  indice = (indice + 1) % LED_COUNT;
			  }
		  }
		  break;

	  case 1:	/* Secuencia 2: los 3 leds parpadean juntos, 300 ms */
		  if (delayRead(&delaySecuencia))
		  {
			  for (uint8_t i = 0; i < LED_COUNT; i++)
			  {
				  toggleLed_GPIO(leds[i]);
			  }
		  }
		  break;

	  case 2:	/* Secuencia 3: periodos independientes por led (100/300/600 ms) */
		  for (uint8_t i = 0; i < LED_COUNT; i++)
		  {
			  if (delayRead(&delayLed[i]))
			  {
				  toggleLed_GPIO(leds[i]);
			  }
		  }
		  break;

	  case 3:	/* Secuencia 4: LD1+LD3 juntos y LD2 inverso, 150 ms */
		  if (delayRead(&delaySecuencia))
		  {
			  ledEncendido = !ledEncendido;
			  if (ledEncendido)
			  {
				  writeLedOn_GPIO(leds[0]);
				  writeLedOff_GPIO(leds[1]);
				  writeLedOn_GPIO(leds[2]);
			  }
			  else
			  {
				  writeLedOff_GPIO(leds[0]);
				  writeLedOn_GPIO(leds[1]);
				  writeLedOff_GPIO(leds[2]);
			  }
		  }
		  break;

	  default:
		  secuencia = 0;
		  break;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief Apaga todos los leds, reinicia las variables de estado e inicializa
  *        todos los retardos. Se ejecuta al iniciar la app y cada vez que se
  *        cambia de secuencia, para arrancar desde una condicion conocida.
  * @retval None
  */
static void reiniciarEstado(void)
{
	for (uint8_t i = 0; i < LED_COUNT; i++)
	{
		writeLedOff_GPIO(leds[i]);
		delayInit(&delayLed[i], periodoLed[i]);
	}
	delayInit(&delaySecuencia, tiempoSecuencia[secuencia]);
	indice = 0;
	ledEncendido = false;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
