/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCP_101_INTA_Pin GPIO_PIN_13
#define MCP_101_INTA_GPIO_Port GPIOC
#define MCP_101_INTA_EXTI_IRQn EXTI15_10_IRQn
#define MCP_101_INTB_Pin GPIO_PIN_0
#define MCP_101_INTB_GPIO_Port GPIOH
#define MCP_101_INTB_EXTI_IRQn EXTI0_IRQn
#define MCP_110_INTB_Pin GPIO_PIN_2
#define MCP_110_INTB_GPIO_Port GPIOC
#define MCP_110_INTB_EXTI_IRQn EXTI2_IRQn
#define MCP_110_INTA_Pin GPIO_PIN_3
#define MCP_110_INTA_GPIO_Port GPIOC
#define MCP_110_INTA_EXTI_IRQn EXTI3_IRQn
#define MCP_111_INTA_Pin GPIO_PIN_4
#define MCP_111_INTA_GPIO_Port GPIOC
#define MCP_111_INTA_EXTI_IRQn EXTI4_IRQn
#define MCP_111_INTB_Pin GPIO_PIN_5
#define MCP_111_INTB_GPIO_Port GPIOC
#define MCP_111_INTB_EXTI_IRQn EXTI9_5_IRQn
#define MCP_011_INTB_Pin GPIO_PIN_1
#define MCP_011_INTB_GPIO_Port GPIOB
#define MCP_011_INTB_EXTI_IRQn EXTI1_IRQn
#define MCP_011_INTA_Pin GPIO_PIN_14
#define MCP_011_INTA_GPIO_Port GPIOB
#define MCP_011_INTA_EXTI_IRQn EXTI15_10_IRQn
#define MCP_100_INTA_Pin GPIO_PIN_15
#define MCP_100_INTA_GPIO_Port GPIOB
#define MCP_100_INTA_EXTI_IRQn EXTI15_10_IRQn
#define MCP_100_INTB_Pin GPIO_PIN_6
#define MCP_100_INTB_GPIO_Port GPIOC
#define MCP_100_INTB_EXTI_IRQn EXTI9_5_IRQn
#define MCP_010_INTA_Pin GPIO_PIN_7
#define MCP_010_INTA_GPIO_Port GPIOC
#define MCP_010_INTA_EXTI_IRQn EXTI9_5_IRQn
#define MCP_010_INTB_Pin GPIO_PIN_8
#define MCP_010_INTB_GPIO_Port GPIOC
#define MCP_010_INTB_EXTI_IRQn EXTI9_5_IRQn
#define MCP_001_INTA_Pin GPIO_PIN_9
#define MCP_001_INTA_GPIO_Port GPIOA
#define MCP_001_INTA_EXTI_IRQn EXTI9_5_IRQn
#define MCP_001_INTB_Pin GPIO_PIN_10
#define MCP_001_INTB_GPIO_Port GPIOA
#define MCP_001_INTB_EXTI_IRQn EXTI15_10_IRQn
#define MCP_000_INTB_Pin GPIO_PIN_11
#define MCP_000_INTB_GPIO_Port GPIOA
#define MCP_000_INTB_EXTI_IRQn EXTI15_10_IRQn
#define MCP_000_INTA_Pin GPIO_PIN_12
#define MCP_000_INTA_GPIO_Port GPIOA
#define MCP_000_INTA_EXTI_IRQn EXTI15_10_IRQn
#define MCP_RST_Pin GPIO_PIN_3
#define MCP_RST_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
