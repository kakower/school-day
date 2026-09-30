/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ESP8266.h"
#include "delay.h"
#include "OLED.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "JQ8900.h"
#include "KEY.h"
#include "DS18B20.h"
#include "HX711.h"
#include "servo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t switch_flag;
struct timer set_time;
uint8_t time_count;
struct timer residue_time = {0, 0, 0};
uint8_t temp_max = 30;
extern uint32_t weight;
uint32_t weight_min = 200;
uint32_t init_weight;
float temp_value;
char s[100];
uint8_t time_flag;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Buzzer_Set(uint8_t x)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (GPIO_PinState)x);
}
void Page(void)
{
  OLED_ShowChinese(1, 1, "��ǰ������");
  memset(s, 0, sizeof(s));
  sprintf(s, "%d g        ", weight);
  OLED_ShowString(80, 1, s, 8);
  memset(s, 0, sizeof(s));
  sprintf(s, "%02d:%02d:%02d", residue_time.hour, residue_time.minute, residue_time.sec);
  OLED_ShowString(1, 16, s, 8);
  OLED_ShowChinese(1, 32, "�¶ȣ�");
  OLED_ShowFloatNum(40, 32, temp_value, 2, 1, 8);
  OLED_ShowChinese(1, 48, "������С��");
  OLED_ShowNum(80, 48, weight_min, 4, 8);
  OLED_ShowNum(80, 16, temp_max, 2, 8);
}

extern struct Record_Info rec;
void Ctrl(void)
{
  if (switch_flag == 0)
  {
    if ((weight < weight_min) || (temp_value > temp_max))
    {
      Buzzer_Set(1);
	  if((weight<weight_min)&& (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) != 1))
	  {
				rec.index = 1;
				strcpy(rec.path, "\"00001\"");
				rec.volume = 20;
				JQ8900_Play_Recording(&rec);
		  Servo_Start(90);
	  }
	  if((temp_value>temp_max)&& (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) != 1))
	  {
				rec.index = 2;
				strcpy(rec.path, "\"00002\"");
				rec.volume = 20;
		  JQ8900_Play_Recording(&rec);
	  }
    }
    else
    {
      Buzzer_Set(0);
      if (weight >= weight_min)
        Servo_Start(0);
    }
  }
}
uint8_t send_time;
void Data_Send(void)
{
  if (send_time < 10)
  {
    send_time++;
    return ;
  }
  send_time = 0;
  memset(s, 0, sizeof(s));
  sprintf(s, "weight:%d,temp:%.1f,\r\n", weight, temp_value);
  ESP8266_SendData(s, strlen(s));
}
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
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  ESP8266_Init();
  HX711_Init_Weight();
  Servo_Start(0);
  Delay_ms(1000);
  HAL_TIM_Base_Start_IT(&htim1);
  Buzzer_Set(0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    ESP8266_GetIPD();
   if ((time_flag == 0 ) && (time_count > 0))
    {
      residue_time.hour = set_time.hour;
      residue_time.minute = set_time.minute ;
      residue_time.sec = set_time.sec;
      time_flag = 1;
      time_count--;
      HAL_TIM_Base_Start_IT(&htim2);
    }
    Key_Scan();
    HX711_GetWeight();
    temp_value = DS18B20_GetTemp();
    Servo_Switch();
    Ctrl();
    Page();
    OLED_Update();
    Data_Send();
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
