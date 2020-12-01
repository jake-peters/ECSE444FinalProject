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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define ARM_MATH_CM4
#include "arm_math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "angles.h"
#include "uart_display.h"
#include "lsm6dsl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac_ch1;

I2C_HandleTypeDef hi2c2;

QSPI_HandleTypeDef hqspi;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

osThreadId orientationTaskHandle;
osThreadId datareadingTaskHandle;
osThreadId gameStartTaskHandle;
osThreadId displayDataTaskHandle;
/* USER CODE BEGIN PV */
typedef enum {
	GAME_START = 1,
	GAME_RUNNING = 2,
	GAME_OVER = 3,
} game_mode_t;

typedef enum {
	TILT_UP,
	TILT_DOWN,
	TILT_RIGHT,
	TILT_LEFT,
	FLAT,
} board_orientation_t;

typedef enum {
	Instruction1,
	Instruction2,
	Instruction3,
	Instruction4,
} Instruction_state_t;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C2_Init(void);
static void MX_QUADSPI_Init(void);
static void MX_USART1_UART_Init(void);
void StartOrientationTask(void const * argument);
void StartDataReadingTask(void const * argument);
void StartGameTask(void const * argument);
void StartDisplayDataTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int16_t aXYZ[3];
int16_t gXYZ[3];
int16_t count = 0;
int16_t hit = 0;
int16_t timecount = 6;
int16_t missile = 0;
static int32_t altitude = 0; // unit in m
static int32_t xaxis = 0; //uint in degrees
static int32_t yaxis = 0; //uint in degrees
static uint16_t speed = 100; // unit in km/h
static int32_t horizontal_position = 0; // units in m, position for left and right
static game_mode_t game_mode = GAME_START; // Initialize game mode to GAME_START
static board_orientation_t orientation = FLAT;	// Current orientation of the board
static Instruction_state_t instruction = Instruction1;
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
  BSP_ACCELERO_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC1_Init();
  MX_TIM2_Init();
  MX_I2C2_Init();
  MX_QUADSPI_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET); // Turn Red LED off


  //Double-tap recognition(Use single-tap to simulate the plane was hit.
  //We didn't choose single-tap recognition as it is so sensitive that very easy to be triggered by mistake.
  // Turn on the accelerometer and set the sampling rate at 416Hz
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL, 0x60);
  //Enable interrupts and tap detection on X, Y, Z axis
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_TAP_CFG1, 0x8E);
  //Set the value of the tap threshold at 562.5mg.
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_TAP_THS_6D, 0x8C);
  //Set the values of Quiet and Shock time windows
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_INT_DUR2, 0x7F);
  //Enable the single-tap recogonition
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_WAKE_UP_THS, 0x80);
  //Let the interrupt generated diven to INT1 pin
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_MD1_CFG, 0x08);


  //free-fall detection using resisters but not work well.
//  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL, 0x60);
//  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_TAP_CFG1, 0x81);
//  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_WAKE_UP_DUR, 0x00);
//  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_FREE_FALL, 0x0F);
//  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_MD1_CFG, 0x10);
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of orientationTask */
  osThreadDef(orientationTask, StartOrientationTask, osPriorityNormal, 0, 128);
  orientationTaskHandle = osThreadCreate(osThread(orientationTask), NULL);

  /* definition and creation of datareadingTask */
  osThreadDef(datareadingTask, StartDataReadingTask, osPriorityNormal, 0, 128);
  datareadingTaskHandle = osThreadCreate(osThread(datareadingTask), NULL);

  /* definition and creation of gameStartTask */
  osThreadDef(gameStartTask, StartGameTask, osPriorityNormal, 0, 128);
  gameStartTaskHandle = osThreadCreate(osThread(gameStartTask), NULL);

  /* definition and creation of displayDataTask */
  osThreadDef(displayDataTask, StartDisplayDataTask, osPriorityIdle, 0, 128);
  displayDataTaskHandle = osThreadCreate(osThread(displayDataTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */
  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x10909CEC;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 255;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 1;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1814;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_G2_GPIO_Port, LED_G2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_R_Pin */
  GPIO_InitStruct.Pin = LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PUSH_BUTTON_Pin */
  GPIO_InitStruct.Pin = PUSH_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PUSH_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_G2_Pin */
  GPIO_InitStruct.Pin = LED_G2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_G2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//Simulate the push-button press as missile was fired
    if(GPIO_Pin == PUSH_BUTTON_Pin){
    	HAL_GPIO_TogglePin(GPIOB, LED_G2_Pin);
    	missile = 1;

    }
    //Simulate when the sensor was double-tap as being hit by other missile
    else if(GPIO_Pin != GPIO_PIN_RESET){
    	hit++;
    	HAL_GPIO_TogglePin(GPIOE, LED_R_Pin);
    }
}

game_mode_t pollStart(game_mode_t game_mode) {
	if (game_mode == GAME_START) {
		while (user_response[0] != 's') { // Keeping displaying prompt until user sends 's'
			HAL_UART_Transmit(&huart1,(uint8_t*)start_menu_message,sizeof(start_menu_message),1000);
			while((HAL_OK != HAL_UART_Receive(&huart1, user_response, 1, 5000))){}
		}
		HAL_UART_Transmit(&huart1,game_starting_message,sizeof(game_starting_message),1000);
		for (uint8_t i = 0; i < 5; i++) { // Countdown to start
			HAL_UART_Transmit(&huart1,&count_down[i],1,1000);
			HAL_UART_Transmit(&huart1,"\n",1,1000);
			osDelay(1000);
		}
		// Start other threads again

		return GAME_RUNNING;
	}
}
void displayInstruction() {

}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartOrientationTask */
/**
* @brief Function implementing the orientationTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOrientationTask */
void StartOrientationTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	    float filterAngle = 0;
		float dt=(1/52);
	    int r = 0;
		int comp_filter(int newAngle,int newRate) {
	    float filterTerm0;
		int filterTerm1;
		float timeConstant;
		filterTerm0 = (newAngle - filterAngle);
		filterAngle = filterTerm0 * 0.93 + newRate*dt*0.07 + filterAngle;
		filterTerm1 = (int)filterAngle;
		r = filterTerm1;
		return r;
		}
  for(;;)
  {
    osDelay(100);
    while(game_mode == GAME_START || game_mode == GAME_OVER) {
    	osDelay(100);
    }

    //read from accelerometer
    //read the value from x-axis and set tilt down condition
	if(aXYZ[0] > 300){
		xaxis = xangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		xaxis = comp_filter(xaxis, gXYZ[1]);
		yaxis = yangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		yaxis = comp_filter(yaxis, gXYZ[0]);
		altitude -= (int)speed*(1/3.6);
		orientation = TILT_DOWN;
	}
	//read the value from x-axis and set tilt up condition
	if(aXYZ[0] < -300){
		xaxis = xangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		xaxis = comp_filter(xaxis, gXYZ[1]);
		yaxis = yangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		yaxis = comp_filter(yaxis, gXYZ[0]);
		altitude += (int)speed*(1/3.6);
		orientation = TILT_UP;
	}
	//read the value from y-axis and set turning left condition
	if(aXYZ[1] > 300){
		xaxis = xangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		xaxis = comp_filter(xaxis, gXYZ[1]);
		yaxis = yangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		yaxis = comp_filter(yaxis, gXYZ[0]);
		horizontal_position -= (int)(speed/20);
		orientation = TILT_LEFT;
	}
	//read the value from y-axis and set turning right condition
	if(aXYZ[1] < -300){
		xaxis = xangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		xaxis = comp_filter(xaxis, gXYZ[1]);
		yaxis = yangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		yaxis = comp_filter(yaxis, gXYZ[0]);
		horizontal_position += (int)(speed/20);
		orientation = TILT_RIGHT;
	}
	//If the board is placed horizontally
	if(aXYZ[1] > -200 && aXYZ[1] < 100 && aXYZ[0] > -200 && aXYZ[0] < 200 && aXYZ[2] > 800 && aXYZ[2] < 1200){
		xaxis = xangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		xaxis = comp_filter(xaxis, gXYZ[1]);
		yaxis = yangles((int)aXYZ[0], (int)aXYZ[1], (int)aXYZ[2]);
		yaxis = comp_filter(yaxis, gXYZ[0]);
		orientation = FLAT;
	}
	if (altitude < 0) {
		game_mode = GAME_OVER;
		HAL_UART_Transmit(&huart1,(uint8_t*)parachute,sizeof(parachute),1000);
		HAL_UART_Transmit(&huart1,(uint8_t*)crash_message,sizeof(crash_message),1000);
	}

	// Temp disable until we know what to do with it
	//Set the condition for the board droped to ground(free-fall)
//	if(aXYZ[1] > -500 && aXYZ[1] < 500 && aXYZ[0] > -500 && aXYZ[0] < 500 && aXYZ[2] > -500 && aXYZ[2] < 500){
//		if(count == 1){
//			char buff5[100];
//			sprintf(buff5, "Crashed! Press the reset button to restart!\n");
//			HAL_UART_Transmit(&huart1, buff5, strlen(buff5), 1000);
//			count = 0;
//			break;
//		}
//		else{
//			count++;
//		}
//	}
	//If we double-tap(simulate the plane was hit) the board for 5 times,
	//the status of the board became game over.
	if(hit == 5){
		char buff6[100];
		sprintf(buff6, "Was shot down! Press the reset button to restart!\n");
		HAL_UART_Transmit(&huart1, buff6, strlen(buff6), 1000);
		hit = 0;
		break;
	}
	if(altitude < 3000){
		instruction = Instruction1;
	}
	if(altitude > 3000 && aXYZ[1] > -200 && aXYZ[1] < 100 && aXYZ[0] > -200 && aXYZ[0] < 200 && aXYZ[2] > 800 && aXYZ[2] < 1200 && hit == 0 && horizontal_position < 60)
		instruction = Instruction2;

  }
    if(altitude > 3000 && horizontal_position > 60){
    	instruction = Instruction3;
    }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartDataReadingTask */
/**
* @brief Function implementing the datareadingTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDataReadingTask */
void StartDataReadingTask(void const * argument)
{
  /* USER CODE BEGIN StartDataReadingTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(10);
    while(game_mode == GAME_START || game_mode == GAME_OVER) {
    	osDelay(50);
    }
    BSP_ACCELERO_AccGetXYZ(aXYZ);
    LSM6DSL_GyroReadXYZAngRate(gXYZ);
    	gXYZ[0] = gXYZ[0]/1000;
   		gXYZ[1] = gXYZ[1]/1000;
    	gXYZ[2] = gXYZ[2]/1000;
  }
  /* USER CODE END StartDataReadingTask */
}

/* USER CODE BEGIN Header_StartGameTask */
/**
* @brief Function implementing the gameStartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGameTask */
void StartGameTask(void const * argument)
{
  /* USER CODE BEGIN StartGameTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
    while(game_mode == GAME_RUNNING || game_mode == GAME_OVER) {
        osDelay(100);
    }
    game_mode = pollStart(game_mode);
  }
  /* USER CODE END StartGameTask */
}

/* USER CODE BEGIN Header_StartDisplayDataTask */
/**
* @brief Function implementing the displayDataTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayDataTask */
void StartDisplayDataTask(void const * argument)
{
  /* USER CODE BEGIN StartDisplayDataTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
    while(game_mode == GAME_START || game_mode == GAME_OVER) {
    	osDelay(100);
    }
    switch (orientation) {
		case TILT_UP: {
			HAL_UART_Transmit(&huart1,(uint8_t*)plane_up,sizeof(plane_up),1000);
			break;
		}
		case TILT_DOWN: {
			HAL_UART_Transmit(&huart1,(uint8_t*)plane_down,sizeof(plane_down),1000);
			break;
		}

		case TILT_RIGHT: {
			HAL_UART_Transmit(&huart1,(uint8_t*)plane_right,sizeof(plane_right),1000);
			break;
		}
		case TILT_LEFT: {
			HAL_UART_Transmit(&huart1,(uint8_t*)plane_left,sizeof(plane_left),1000);
			break;
		}
		case FLAT: {
			HAL_UART_Transmit(&huart1,(uint8_t*)plane_straight,sizeof(plane_straight),1000);
			break;
		}
    }

    switch (instruction){
	case Instruction1: {
		if (altitude < 3000){
		HAL_UART_Transmit(&huart1,(uint8_t*)instruction1,sizeof(instruction1),1000);
				if(!(xaxis > 30 && xaxis < 35)){
					HAL_UART_Transmit(&huart1,(uint8_t*)warning1,sizeof(warning1),1000);
				}
		}
		break;
	}
	case Instruction2: {
		if(horizontal_position < 60){
		if(hit == 0){
		timecount = timecount - 1;
		instruction2countdown(&instruction2,timecount);
		HAL_UART_Transmit(&huart1,(uint8_t*)instruction2,sizeof(instruction2),1000);
		}
		if(hit == 1){
			HAL_UART_Transmit(&huart1,(uint8_t*)warning2,sizeof(warning2),1000);
		}
		if (timecount == 0 && hit == 0){
			game_mode = GAME_OVER;
			HAL_UART_Transmit(&huart1,(uint8_t*)parachute,sizeof(parachute),1000);
			HAL_UART_Transmit(&huart1,(uint8_t*)crash_message,sizeof(crash_message),1000);
		}
		}
		break;
	}

	case Instruction3: {
		if(horizontal_position < 300){
				HAL_UART_Transmit(&huart1,(uint8_t*)instruction33,sizeof(instruction33),1000);
		       if(!(yaxis > 42 && yaxis < 48)){
					HAL_UART_Transmit(&huart1,(uint8_t*)warning1,sizeof(warning1),1000);
						}
		}
		break;
	}
	case Instruction4: {
		HAL_UART_Transmit(&huart1,(uint8_t*)plane_left,sizeof(plane_left),1000);
		break;
	}
    }
	HAL_UART_Transmit(&huart1,(uint8_t*)fd_title,sizeof(fd_title),1000);
	flightDataPreviousRow(&fd_previous_row, xaxis);
	flightDataPrevious2Row(&fd_previous2_row, yaxis);
	flightDataFirstRow(&fd_first_row, altitude, speed);
	flightDataSecondtRow(&fd_second_row, horizontal_position);
	flightDataThirdRow(&fd_third_row, "Do a barrel roll", 23);
	HAL_UART_Transmit(&huart1,(uint8_t*)fd_previous_row,sizeof(fd_previous_row),1000);
	HAL_UART_Transmit(&huart1,(uint8_t*)fd_previous2_row,sizeof(fd_previous2_row),1000);
	HAL_UART_Transmit(&huart1,(uint8_t*)fd_first_row,sizeof(fd_first_row),1000);
	HAL_UART_Transmit(&huart1,(uint8_t*)fd_second_row,sizeof(fd_second_row),1000);
	HAL_UART_Transmit(&huart1,(uint8_t*)fd_third_row,sizeof(fd_third_row),1000);
  }
  /* USER CODE END StartDisplayDataTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
