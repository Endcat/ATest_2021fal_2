/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DS18B20_DQ_H 				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET)
#define DS18B20_DQ_L 				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET)
#define DS18B20_DQ_ReadPin 	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)

#define DIR_ReadPin					HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)
#define DIR_H								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define DIR_L								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define PWM_H								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)
#define PWM_L								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
uint8_t const table1[] = "Temp=";
uint8_t const table2[] = "DC Motor opened";
uint8_t const table3[] = "DC Motor closed";
char buff[16];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void Delay_us(uint16_t us)
{
	uint16_t differ=0xffff-us-5;
	
	__HAL_TIM_SET_COUNTER(&htim3,differ);
	
	HAL_TIM_Base_Start(&htim3);
	
	while(differ < 0xffff-6)
		differ = __HAL_TIM_GET_COUNTER(&htim3);
	
	HAL_TIM_Base_Stop(&htim3);
}

void LcdWriteCom(uint8_t com)
{
	Delay_us(20);
	GPIOA->BSRR = 0x00ff0000;
	GPIOA->BSRR = (com);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	Delay_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
	Delay_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	Delay_us(10);
}

void LcdWriteDate(uint8_t date)
{
	Delay_us(20);
	GPIOA->BSRR = 0x00ff0000;
	GPIOA->BSRR = (date);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	Delay_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
	Delay_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	Delay_us(10);
	
}

void LCD1602Init(void)
{
	uint8_t index = 0;
	HAL_Delay(100);
	LcdWriteCom(0x38);
	LcdWriteCom(0x0c);
	LcdWriteCom(0x06);
	LcdWriteCom(0x01);
	HAL_Delay(100);
	
	LcdWriteCom(0x80);
	// here length(table1) == 5, idk why index < 13, so i modified
	// maybe the former string in table1 is "temperature= " (<13)
	for (index = 0; index < 5; index++)
		LcdWriteDate(table1[index]);
}

void LCD1602WriteCommand(uint8_t comm)
{
	LcdWriteCom(0xc0 + 14);
	LcdWriteDate(comm);
}

void DS18B20_DQ_DDR(uint8_t ddr)
{
	if (ddr == 1)
	{
		GPIOA->CRH &= 0xFFFF1FFF;
		GPIOA->CRH |= 0x00001000;
	}
	else
	{
		GPIOA->CRH &= 0xFFFF8FFF;
		GPIOA->CRH |= 0x00008000;
	}
}

uint8_t DS18B20_Rbit(void)
{
	uint8_t date, x;
	DS18B20_DQ_DDR(1);
	DS18B20_DQ_L;
	Delay_us(2);
	DS18B20_DQ_H;
	DS18B20_DQ_DDR(0);
	Delay_us(12);
	x = DS18B20_DQ_ReadPin;
	if (x)
		date = 0x80;
	else
		date = 0;
	Delay_us(50);
	return date;
}

uint8_t DS18B20_Rbyte(void)
{
	uint8_t rbyte = 0, i = 0, tempbit = 0;
	for (i = 1; i <= 8; i++)
	{
		tempbit = DS18B20_Rbit();
		rbyte = rbyte >> 1;
		rbyte = rbyte | tempbit;
	}
	return rbyte;
}

void DS18B20_Wbyte(uint8_t xbyte)
{
	uint8_t i, x = 0;
	DS18B20_DQ_DDR(1);
	for (i = 1; i <= 8; i++)
	{
		x = xbyte & 0x01;
		if (x)
		{
			DS18B20_DQ_L;
			Delay_us(2);
			DS18B20_DQ_H;
			Delay_us(60);
		}
		else
		{
			DS18B20_DQ_L;
			Delay_us(60);
			DS18B20_DQ_H;
			Delay_us(2);
		}
		xbyte = xbyte >> 1;
	}
}

void DS18B20_Reset(void)
{
	DS18B20_DQ_DDR(1);
	DS18B20_DQ_H;
	Delay_us(700);
	DS18B20_DQ_L;
	Delay_us(500);
	DS18B20_DQ_DDR(0);
	DS18B20_DQ_H;
	Delay_us(40);
	Delay_us(33);
	DS18B20_DQ_ReadPin;
	Delay_us(500);
}

short DS18B20_Get_Temp(void)
{
	uint8_t fg;
	uint8_t TL, TH;
	short data;
	
	DS18B20_Reset();
	DS18B20_Wbyte(0xcc);
	DS18B20_Wbyte(0x44);
	// modified
	//TL = DS18B20_Rbyte();
	DS18B20_Reset();
	DS18B20_Wbyte(0xcc);
	DS18B20_Wbyte(0xbe);
	TL = DS18B20_Rbyte();
	TH = DS18B20_Rbyte();
	
	if (TH > 0x7)
	{
		TL = ~TL;
		TH = ~TH;
		fg = 0;
	}
	else
		fg = 1;
	
	data = TH;
	data <<= 8;
	data += TL;
	data = (float)data * 0.0625;
	if(fg)
		return data;
	else
		return -data;
}
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
	uint8_t i, index;
	int8_t temp;
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	LCD1602Init(); // print "temp=" successfully
	DS18B20_Get_Temp();
	printf("Testing OK\r");
	HAL_Delay(800);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
		temp = DS18B20_Get_Temp();
		
		DIR_L;
		sprintf(&buff[0], "Temp=%dC CW ",temp);
		LcdWriteCom(0x80);
		for (i = 0; i < strlen(buff); i++)
			LcdWriteDate(buff[i]);
		
		LcdWriteCom(0xc0);
		if (temp > 31) // set my student number
		{
			// opened
			for (index = 0; index < 15; index++)
				LcdWriteDate(table2[index]);
			
			PWM_H;
			HAL_Delay(70);
			PWM_L;
			HAL_Delay(30);
		}
		else
		{
			// closed
			for (index = 0; index < 15; index++)
				LcdWriteDate(table3[index]);
			
			PWM_H;
			HAL_Delay(100);
		}
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15, GPIO_PIN_RESET);

   GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
