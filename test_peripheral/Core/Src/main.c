/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "../../app/Screen.h"

#include "../../lib/AT24Cxx.h"
#include "../../os/os.h"

#include "../../app/app_main.h"

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
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;

uint8_t TxData[8];
uint8_t RxData[8];

uint32_t TxMailbox;

uint8_t dataRX[100];
uint8_t data_output[20] ={0};
char *data_test[] = {
  "$4053"                // NAME
  "Description"          // Description
  "16"                   // Num pin
  "16"                   // Num case
  "11L1100G0001011V",
  "11L1100G0011101V",
  "11L1100G0101110V",
  "11L1100G0110111V",
  "01L1100G1001111V",
  "11L1000G1011111V",
  "10L1100G1101111V",
  "11L0100G1111111V"
};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */


  HAL_Delay(3000);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, dataRX, sizeof(dataRX)); // Enable interrupt UART

  Screen_begin(&huart2);

//  HAL_I2C_IsDeviceReady(&hi2c1,0x50 << 1, 1, 100);

//  uint8_t test_eeprom_buf[500] = {0};
//  uint8_t test_write_eeprom_buf[10] = {1,2,3,4,5,6,7,8,9,10};
  AT24Cxx_t pEeprom;
  AT24Cxx_Init(&pEeprom, 0x50, &hi2c1);
//  char text_program_1[] = "0";
//  char *text_program_2 = "Program test2,74hc560,20";
//  char *text_program_3 = "Program test3,74hc580,40";
//  char *text_program_4 = "Program test4,74hc590,50";
//  AT24Cxx_read_buffer(&pEeprom,0x10,test_eeprom_buf,10);
//  HAL_I2C_Mem_Write_IT(&hi2c1,(0x50 << 1),0x02,I2C_MEMADD_SIZE_16BIT,text_program_1,strlen(text_program_1));
//  char clear_buf[TOTAL_ALL_PROGRAM_TEST_LEN] = {0};
//  memset(clear_buf,0xFF,TOTAL_ALL_PROGRAM_TEST_LEN);
  // AT24Cxx_write_buffer_bloking(&pEeprom,START_MEM_ADDR_PROGRAM_TEST,(uint8_t *)clear_buf,TOTAL_ALL_PROGRAM_TEST_LEN);
  // AT24Cxx_write_buffer(&pEeprom,START_ADDR_PROGRAM_TEST_X(0),test_write_eeprom_buf,10);
  //  HAL_Delay(10);
//  AT24Cxx_read_buffer(&pEeprom,START_ADDR_PROGRAM_TEST_X(0),test_eeprom_buf,TOTAL_ONE_PROGRAM_TEST_LEN);
  // AT24Cxx_read_buffer(&pEeprom,START_MEM_ADDR_PROGRAM_TEST,test_eeprom_buf,TOTAL_ALL_PROGRAM_TEST_LEN);
  // AT24Cxx_write_buffer_bloking(&pEeprom,START_ADDR_PROGRAM_TEST_X(0),(uint8_t *)text_program_1,strlen(text_program_1));
  // HAL_Delay(10);
  // AT24Cxx_read_buffer(&pEeprom,START_MEM_ADDR_PROGRAM_TEST,test_eeprom_buf,TOTAL_ALL_PROGRAM_TEST_LEN);
  // AT24Cxx_write_buffer_bloking(&pEeprom,START_ADDR_PROGRAM_TEST_X(1),(uint8_t *)text_program_2,strlen(text_program_2));
  // HAL_Delay(10);
  // AT24Cxx_read_buffer(&pEeprom,START_MEM_ADDR_PROGRAM_TEST,test_eeprom_buf,TOTAL_ALL_PROGRAM_TEST_LEN);
  // AT24Cxx_write_buffer_bloking(&pEeprom,START_ADDR_PROGRAM_TEST_X(2),(uint8_t *)text_program_3,strlen(text_program_3));
  // HAL_Delay(10);
  // AT24Cxx_read_buffer(&pEeprom,START_MEM_ADDR_PROGRAM_TEST,test_eeprom_buf,TOTAL_ALL_PROGRAM_TEST_LEN);
  // AT24Cxx_write_buffer_bloking(&pEeprom,START_ADDR_PROGRAM_TEST_X(3),(uint8_t *)text_program_4,strlen(text_program_4));
  // HAL_Delay(10);
  // AT24Cxx_write_buffer(&pEeprom,0x02,(uint8_t *)text_program_1,strlen(text_program_1));

//  AT24Cxx_read_buffer(&pEeprom,START_MEM_ADDR_PROGRAM_TEST,test_eeprom_buf,TOTAL_ALL_PROGRAM_TEST_LEN);


// pEeprom.i2c_port = (I2C_HandleTypeDef*)0;
// pEeprom.dev_address = 0;
//  for (uint8_t i = 0; i < 5; i++) {
//    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
//    HAL_Delay(200);
//    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
//    HAL_Delay(200);
//	printf("SWO Debug!!!\n");
//  }

 BSP_init();

 Post_task_init();
 static OS_event_t const *q_app_post[10];
 OS_task_create(AO_taskPost,
   1,
   q_app_post,
   ARRAY_ELEMENT(q_app_post),
   (OS_event_t *)0 );

 eeprom_task_init(&hi2c1,EEPROM_ADDRESS);
 static OS_event_t const *q_eeprom_event[10];
 OS_task_create( AO_task_eeprom,
   1,
   q_eeprom_event,
   ARRAY_ELEMENT(q_eeprom_event),
   (OS_event_t *)0 );

  uart_esp32_task_init(&huart1);
  static OS_event_t const *q_uart_esp32_event[10];
  OS_task_create(AO_task_uart_esp32,
  1,
  q_uart_esp32_event,
  ARRAY_ELEMENT(q_uart_esp32_event),
  (OS_event_t *)0);

  BlinkyTest_app_init();
  static OS_event_t const *TestOS_blinky[10]; /* Event queue */
  OS_task_create(
    AO_BlinkyTest,
    1,
    TestOS_blinky,
    ARRAY_ELEMENT(TestOS_blinky),
    TestOS_Work()
  );

  app_can_bus_init(&hcan);
  static OS_event_t const *Can_app_event[10];
  OS_task_create( AO_task_can_bus,
  1,
  Can_app_event,
  ARRAY_ELEMENT(Can_app_event),
  (OS_event_t *)0);


  //    uint8_t sendBuffer_3[] = {
  //   0x5A, 0xA5, 0xFB, 0x82,  
  //   0x19, 0x00, 0x00, 0x02,
  //   0x00, 0x3B, 0x01, 0xED,

  //   // Clock 1
  //   0x00, 0x19, 0x01, 0x18,  0x00, 0x3C, 0x01, 0x18,
  //   0x00, 0x3C, 0x00, 0xFA,  0x00, 0x5F, 0x00, 0xFA,

  //   // Clock 2
  //   0x00, 0x5F, 0x01, 0x18,  0x00, 0x82, 0x01, 0x18,
  //   0x00, 0x82, 0x00, 0xFA,  0x00, 0xA5, 0x00, 0xFA,

  //   // Clock 3
  //   0x00, 0xA5, 0x01, 0x18,  0x00, 0xC8, 0x01, 0x18,
  //   0x00, 0xC8, 0x00, 0xFA,  0x00, 0xEB, 0x00, 0xFA,

  //   // Clock 4
  //   0x00, 0xEB, 0x01, 0x18,  0x01, 0x0E, 0x01, 0x18,
  //   0x01, 0x0E, 0x00, 0xFA,  0x01, 0x31, 0x00, 0xFA,

  //   // Clock 5
  //   0x01, 0x31, 0x01, 0x18,  0x01, 0x54, 0x01, 0x18,
  //   0x01, 0x54, 0x00, 0xFA,  0x01, 0x77, 0x00, 0xFA,

  //   // Clock 6
  //   0x01, 0x77, 0x01, 0x18,  0x01, 0x9A, 0x01, 0x18,
  //   0x01, 0x9A, 0x00, 0xFA,  0x01, 0xBD, 0x00, 0xFA,

  //   // Clock 7
  //   0x01, 0xBD, 0x01, 0x18,  0x01, 0xE0, 0x01, 0x18,
  //   0x01, 0xE0, 0x00, 0xFA,  0x02, 0x03, 0x00, 0xFA,

  //   // Clock 8
  //   0x02, 0x03, 0x01, 0x18,  0x02, 0x26, 0x01, 0x18,
  //   0x02, 0x26, 0x00, 0xFA,  0x02, 0x49, 0x00, 0xFA,

  //   // Clock 9
  //   0x02, 0x49, 0x01, 0x18,  0x02, 0x6C, 0x01, 0x18,
  //   0x02, 0x6C, 0x00, 0xFA,  0x02, 0x8F, 0x00, 0xFA,

  //   // Clock 10
  //   0x02, 0x8F, 0x01, 0x18,  0x02, 0xB2, 0x01, 0x18,
  //   0x02, 0xB2, 0x00, 0xFA,  0x02, 0xD5, 0x00, 0xFA,

  //   // Clock 11
  //   0x02, 0xD5, 0x01, 0x18,  0x02, 0xF8, 0x01, 0x18,
  //   0x02, 0xF8, 0x00, 0xFA,  0x03, 0x1B, 0x00, 0xFA,

  //   // Clock 12
  //   0x03, 0x1B, 0x01, 0x18,  0x03, 0x3E, 0x01, 0x18,
  //   0x03, 0x3E, 0x00, 0xFA,  0x03, 0x61, 0x00, 0xFA,

  //   // Clock 13
  //   0x03, 0x61, 0x01, 0x18,  0x03, 0x84, 0x01, 0x18,
  //   0x03, 0x84, 0x00, 0xFA,  0x03, 0xA7, 0x00, 0xFA,

  //   // Clock 14
  //   0x03, 0xA7, 0x01, 0x18,  0x03, 0xCA, 0x01, 0x18,
  //   0x03, 0xCA, 0x00, 0xFA,  0x03, 0xED, 0x00, 0xFA,

  //   // Clock 15
  //   0x03, 0xED, 0x01, 0x18,  0x04, 0x10, 0x01, 0x18,
  //   0x04, 0x10, 0x00, 0xFA,  0x04, 0x33, 0x00, 0xFA,

  //   0xFF, 0x00};

  //  HAL_UART_Transmit(&huart2,sendBuffer_3,sizeof(sendBuffer_3),500);
//  for (uint8_t i=0; i < 6; i++) {
//  			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
//  			  HAL_Delay(500);
//  		  }
  HAL_CAN_Start(&hcan);

  TxHeader.DLC = 5;
  TxHeader.StdId = 0x103;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;

  uint8_t data_tx[5] = {'H','E','L','L','O'};


  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);


     //HAL_CAN_AddTxMessage(&hcan,&TxHeader, data_tx, &TxMailbox);

  // TxHeader.DLC = 2; // data length
  // TxHeader.IDE = CAN_ID_STD;
  // TxHeader.RTR = CAN_RTR_DATA;
  // TxHeader.StdId = 0x407;

	// TxData[0] = 50;
	// TxData[1] = 55;

	// Send data
	// HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	     //HAL_CAN_AddTxMessage(&hcan,&TxHeader, data_tx, &TxMailbox);
	     //HAL_Delay(2000);
	  // if( datacheck ) {
		//   for (uint8_t i=0; i < RxData[1]; i++) {
		// 	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
		// 	  HAL_Delay(RxData[0]);
		//   }
		//   datacheck = 0;

		//   TxData[0] = RxData[0];
		//   TxData[1] = RxData[1];

		//   HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  // }
  //  uint8_t sendBuffer[] = {0x5A, 0xA5, 0x13, 0x82, 0x19, 0x00, 0x00, 0x04, 0x00, 0x01,
  //               0x00, 0x64, 0x00, 0x96, // 100;150
  //               0x00, 0x82, 0x01, 0x2C, // 130;300
  //               0xF8, 0x00, 0xFF, 0x00};
  // HAL_UART_Transmit(&huart2,sendBuffer,sizeof(sendBuffer),500);
  // HAL_Delay(500);
  //  uint8_t sendBuffer_1[100] = {0x5A, 0xA5, 0x13, 0x82, 0x19, 0x00, 0x00, 0x04, 0x00, 0x01,
  //               0x00, 0x64, 0x00, 0xB4, // 100;180
  //               0x00, 0x82, 0x01, 0x2C, // 130;300
  //               0xF8, 0x00, 0xFF, 0x00};
  // HAL_UART_Transmit(&huart2,sendBuffer_1,sizeof(sendBuffer_1),500);
  // HAL_Delay(500);
  //  uint8_t sendBuffer_2[100] = {0x5A, 0xA5, 0x13, 0x82, 0x19, 0x00, 0x00, 0x04, 0x00, 0x01,
  //             0x00, 0x64, 0x00, 0xD2, // 100;210
  //             0x00, 0x82, 0x01, 0x2C, // 130;300
  //             0xF8, 0x00, 0xFF, 0x00};
  // HAL_UART_Transmit(&huart2,sendBuffer_2,sizeof(sendBuffer_2),500);
  // HAL_Delay(500);
  //  uint8_t sendBuffer_3[100] = {0x5A, 0xA5, 0x13, 0x82, 0x19, 0x00, 0x00, 0x04, 0x00, 0x01,
  //           0x00, 0x64, 0x00, 0xF0, // 100;240
  //           0x00, 0x82, 0x01, 0x2C, // 130;300
  //           0xF8, 0x00, 0xFF, 0x00};
  // HAL_UART_Transmit(&huart2,sendBuffer_3,sizeof(sendBuffer_3),500);
  // HAL_Delay(500);

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
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  CAN_FilterTypeDef canfilterconfig;

    canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig.FilterBank = 10;  // which filter bank to use from the assigned ones
    canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
    canfilterconfig.FilterIdHigh = 0x103<<5;
    canfilterconfig.FilterIdLow = 0;
    canfilterconfig.FilterMaskIdHigh = 0;
    canfilterconfig.FilterMaskIdLow = 0;
    canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig.SlaveStartFilterBank = 0;

    HAL_CAN_ConfigFilter(&hcan, &canfilterconfig);
  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 300000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 460800;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if( huart->Instance == huart2.Instance ) {

		// Call to callback function screen
		Screen_RX_data(dataRX);

		// Clear RX Buffer
		memset(dataRX,0,sizeof(dataRX));

		// Enable Interrupt UART again
		HAL_UARTEx_ReceiveToIdle_IT(&huart2, dataRX, sizeof(dataRX)); // Enable interrupt UART
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == hi2c1.Instance) {

		
	}
  /* Transmit complete wait eeprom write */
  OS_task_post_event(AO_task_eeprom, WAIT_WRITE_TIMEOUT, (uint8_t *)0, 0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData);
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
    return len;
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
