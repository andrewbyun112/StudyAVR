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

/*      
         ARM EXAM
         Created : 2020-06-25
         Author :  변승재
*/

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SHT2x_ADDR                      0x40<<1         //slave주소가 7비트라서 시프트1
#define SHT2x_HOLD_MASTER_T             0xE3
#define SHT2x_HOLD_MASTER_RH            0xE5
#define SHT2x_NOHOLD_MASTER_T           0xF3
#define SHT2x_NOHOLD_MASTER_RH          0xF5
#define SHT2x_WRITE_USER_REG            0xE6
#define SHT2x_READ_USER_REG             0xE7
#define SHT2x_SOFT_RESET                0xFE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned char rxData = 0;        //입력값 val
unsigned int menuData = 0;      //menu val

unsigned int CCR1_Val = 0;      //led val

uint16_t adcData = 0;                //adc val
char str1[10];                         //adc str

uint8_t i2cData[2];                   //i2c val
uint16_t val = 0;                       //i2c val
uint8_t mode;                          //i2c val
float TEMP, HUMI;                   //i2c val
char str2[16];                        //Temp&Humi str

//unsigned int music_val = 0;     //음계 val
unsigned int DoReMi[8] = {261, 293, 329, 349, 391, 440, 493, 523};     //음계
unsigned long pwmfreq;
unsigned piano = 0;

uint8_t Time_STOP = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

int fputc(int ch, FILE *stream)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10);
  return ch;
}

void LEDMenu();
void ADCMenu();
void TEMPMenu();

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  lcdInit();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //tim2 ch1설정
  HAL_UART_Receive_IT(&huart1, &rxData, 1); //인터럽트 OPEN
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adcData, sizeof(adcData)); //cds_DMA
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); //tim3 ch1설정
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("\r\n++++ Menu ++++\r\n L : LED\r\n C : CDS\r\n T : TEMP&HUMI\r\n 1 ~ 8 : Digital Piano\r\n push S/W : STOP\r\n");
  
  while (1)
  {
//---------------- LED ------------------//  
    //if(menuData == 1) {
    if(rxData == 'l' || rxData == 'L') {
      if(Time_STOP == 0) {
        LEDMenu();
        
      }
    }
//---------------- ADC ------------------//
    
    if(rxData == 'c' || rxData == 'C') {
      if(Time_STOP == 0) {
        ADCMenu();
      }
    }
//---------------- TEMP & HUMID ------------------//
    if(rxData == 't' || rxData == 'T') {
      if(Time_STOP == 0) {
        TEMPMenu();
      }
    }
//---------------- Piano ------------------//
    //pwmfreq = 0;
    //TIM3->ARR = 0;
    //TIM3->CCR1 = 0;
    
    if(rxData == '1') {         //도
      piano = 1;
      if(piano == 1){
        pwmfreq = 8400000/DoReMi[0];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/10;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '2') {         //레
      piano = 2;
      if(piano == 2){
        pwmfreq = 8400000/DoReMi[1];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '3') {         //미
      piano = 3;
      if(piano == 3){
        pwmfreq = 8400000/DoReMi[2];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '4') {         //파
      piano = 4;
      if(piano == 4){
        pwmfreq = 8400000/DoReMi[3];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '5') {         //솔
      piano = 5;
      if(piano == 5){
        pwmfreq = 8400000/DoReMi[4];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '6') {         //라
      piano = 6;
      if(piano == 6){
        pwmfreq = 8400000/DoReMi[5];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '7') {         //시
      piano = 7;
      if(piano == 7){
        pwmfreq = 8400000/DoReMi[6];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
    }
    
    if(rxData == '8') {         //도
      piano = 8;
      if(piano == 8){
        pwmfreq = 8400000/DoReMi[7];
        TIM3->ARR = pwmfreq -1;
        TIM3->CCR1 = pwmfreq/2;
        HAL_Delay(500);
        TIM3->ARR = 0;
        TIM3->CCR1 = 0;
        piano = 0;
        rxData = 0;
      }
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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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

/* USER CODE BEGIN 4 */
//----------------LED FUNCTION-----------------
void LEDMenu() {
  TIM2->CCR1 = CCR1_Val;
   CCR1_Val++;
   if(CCR1_Val > 100) CCR1_Val = 0;
   HAL_Delay(50);
}
//----------------ADC FUNCTION-----------------
void ADCMenu() {
  lcdClear();
  lcdGotoXY(0,0);
  lcdPrintData("ADC DMA_Value: ",13);
  lcdGotoXY(0,1);
  sprintf(str1, "ADC : %d", adcData);
  lcdPrint(str1);
  HAL_Delay(500);
}
//----------------TEMP & HUMID FUNCTION-----------------
void TEMPMenu() {
  mode = SHT2x_NOHOLD_MASTER_T;
  HAL_I2C_Master_Transmit(&hi2c1, SHT2x_ADDR, &mode, 1, 10);
  HAL_Delay(100);
  HAL_I2C_Master_Receive(&hi2c1, SHT2x_ADDR, i2cData, 1, 10);
  val = i2cData[0] << 8 | i2cData[1];
  TEMP = -46.85 +175.72 * ((float)val / 65536);
  lcdGotoXY(0,0);
  sprintf(str2, "TEMP : %5.1lf ", TEMP);
  lcdPrint(str2);
    
  mode = SHT2x_NOHOLD_MASTER_RH;
  HAL_I2C_Master_Transmit(&hi2c1, SHT2x_ADDR, &mode, 1, 10);
  HAL_Delay(100);
  HAL_I2C_Master_Receive(&hi2c1, SHT2x_ADDR, i2cData, 1, 10);
  val = i2cData[0] << 8 |i2cData[1];
  HUMI = -6 +125 * ((float)val / 65536);
  lcdGotoXY(0,1);
  sprintf(str2, "HUMID : %5.1lf ", HUMI);
  lcdPrint(str2);
}
//----------------UART CALLBACK FUNCTION-----------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1) {
    HAL_UART_Receive_IT(&huart1, &rxData, 1); //수신은 인터럽트(IT)로 받음
    HAL_UART_Transmit(&huart1, &rxData, 1, 10);
  }
}
//----------------SWITCH CALLBACK FUNCTION----------------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(Time_STOP == 0) Time_STOP = 1;
  else Time_STOP = 0;
}
//---------------------------------------------------------
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
