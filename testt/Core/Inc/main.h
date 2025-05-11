/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Control_IC.h"
#include "Testing_program.h"


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void delay_us(uint16_t delay_us);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TXS_OE1_Pin GPIO_PIN_7
#define TXS_OE1_GPIO_Port GPIOE
#define OUT8_Pin GPIO_PIN_8
#define OUT8_GPIO_Port GPIOE
#define OUT7_Pin GPIO_PIN_9
#define OUT7_GPIO_Port GPIOE
#define OUT6_Pin GPIO_PIN_10
#define OUT6_GPIO_Port GPIOE
#define OUT5_Pin GPIO_PIN_11
#define OUT5_GPIO_Port GPIOE
#define OUT4_Pin GPIO_PIN_12
#define OUT4_GPIO_Port GPIOE
#define OUT3_Pin GPIO_PIN_13
#define OUT3_GPIO_Port GPIOE
#define OUT2_Pin GPIO_PIN_14
#define OUT2_GPIO_Port GPIOE
#define OUT1_Pin GPIO_PIN_15
#define OUT1_GPIO_Port GPIOE
#define OUT12_Pin GPIO_PIN_10
#define OUT12_GPIO_Port GPIOB
#define OUT11_Pin GPIO_PIN_11
#define OUT11_GPIO_Port GPIOB
#define OUT10_Pin GPIO_PIN_12
#define OUT10_GPIO_Port GPIOB
#define OUT9_Pin GPIO_PIN_13
#define OUT9_GPIO_Port GPIOB
#define TXS_OE2_Pin GPIO_PIN_8
#define TXS_OE2_GPIO_Port GPIOD
#define TXS_OE4_Pin GPIO_PIN_9
#define TXS_OE4_GPIO_Port GPIOD
#define OUT20_Pin GPIO_PIN_10
#define OUT20_GPIO_Port GPIOD
#define OUT19_Pin GPIO_PIN_11
#define OUT19_GPIO_Port GPIOD
#define OUT18_Pin GPIO_PIN_12
#define OUT18_GPIO_Port GPIOD
#define OUT17_Pin GPIO_PIN_13
#define OUT17_GPIO_Port GPIOD
#define OUT16_Pin GPIO_PIN_14
#define OUT16_GPIO_Port GPIOD
#define OUT15_Pin GPIO_PIN_15
#define OUT15_GPIO_Port GPIOD
#define OUT14_Pin GPIO_PIN_6
#define OUT14_GPIO_Port GPIOC
#define OUT13_Pin GPIO_PIN_7
#define OUT13_GPIO_Port GPIOC
#define TXS_OE3_Pin GPIO_PIN_11
#define TXS_OE3_GPIO_Port GPIOC
#define IN7_Pin GPIO_PIN_0
#define IN7_GPIO_Port GPIOD
#define IN6_Pin GPIO_PIN_1
#define IN6_GPIO_Port GPIOD
#define IN5_Pin GPIO_PIN_2
#define IN5_GPIO_Port GPIOD
#define IN4_Pin GPIO_PIN_3
#define IN4_GPIO_Port GPIOD
#define IN3_Pin GPIO_PIN_4
#define IN3_GPIO_Port GPIOD
#define IN2_Pin GPIO_PIN_5
#define IN2_GPIO_Port GPIOD
#define IN1_Pin GPIO_PIN_6
#define IN1_GPIO_Port GPIOD
#define IN0_Pin GPIO_PIN_7
#define IN0_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
