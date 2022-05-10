/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "callback.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  __HAL_GPIO_EXTI_CLEAR_IT(MCP_101_INTB_Pin);
  GPIO_EXTI0_Callback();

  /* USER CODE END EXTI0_IRQn 0 */
  //HAL_GPIO_EXTI_IRQHandler(MCP_101_INTB_Pin);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

    __HAL_GPIO_EXTI_CLEAR_IT(MCP_011_INTB_Pin);
    GPIO_EXTI1_Callback();

  /* USER CODE END EXTI1_IRQn 0 */
  //HAL_GPIO_EXTI_IRQHandler(MCP_011_INTB_Pin);
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */

    __HAL_GPIO_EXTI_CLEAR_IT(MCP_110_INTB_Pin);
    GPIO_EXTI2_Callback();


  /* USER CODE END EXTI2_IRQn 0 */
  //HAL_GPIO_EXTI_IRQHandler(MCP_110_INTB_Pin);
  /* USER CODE BEGIN EXTI2_IRQn 1 */

  /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

    __HAL_GPIO_EXTI_CLEAR_IT(MCP_110_INTA_Pin);
    GPIO_EXTI3_Callback();

  /* USER CODE END EXTI3_IRQn 0 */
  // HAL_GPIO_EXTI_IRQHandler(MCP_110_INTA_Pin);
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

    __HAL_GPIO_EXTI_CLEAR_IT(MCP_111_INTA_Pin);
    GPIO_EXTI4_Callback();

  /* USER CODE END EXTI4_IRQn 0 */
  //HAL_GPIO_EXTI_IRQHandler(MCP_111_INTA_Pin);
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream4 global interrupt.
  */
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream4_IRQn 0 */

  /* USER CODE END DMA1_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
  /* USER CODE BEGIN DMA1_Stream4_IRQn 1 */

  /* USER CODE END DMA1_Stream4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi3_tx);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

	if(__HAL_GPIO_EXTI_GET_IT(MCP_111_INTB_Pin) != RESET){
		  __HAL_GPIO_EXTI_CLEAR_IT(MCP_111_INTB_Pin);
		  GPIO_EXTI5_Callback();
	  }
	  else if(__HAL_GPIO_EXTI_GET_IT(MCP_100_INTB_Pin) != RESET){
		  __HAL_GPIO_EXTI_CLEAR_IT(MCP_100_INTB_Pin);
		  GPIO_EXTI6_Callback();
	  }
	  else if(__HAL_GPIO_EXTI_GET_IT(MCP_010_INTA_Pin) != RESET){
		  __HAL_GPIO_EXTI_CLEAR_IT(MCP_010_INTA_Pin);
		  GPIO_EXTI7_Callback();
	  }
	  else if(__HAL_GPIO_EXTI_GET_IT(MCP_010_INTB_Pin) != RESET){
		  __HAL_GPIO_EXTI_CLEAR_IT(MCP_010_INTB_Pin);
		  GPIO_EXTI8_Callback();
	  }
	  else if(__HAL_GPIO_EXTI_GET_IT(MCP_001_INTA_Pin) != RESET){
		  __HAL_GPIO_EXTI_CLEAR_IT(MCP_001_INTA_Pin);
		  GPIO_EXTI9_Callback();
	  }

  /* USER CODE END EXTI9_5_IRQn 0 */
	TODO 4 mai : supprimer les appels dans cubemx
  /*HAL_GPIO_EXTI_IRQHandler(MCP_111_INTB_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_100_INTB_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_010_INTA_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_010_INTB_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_001_INTA_Pin);*/
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  if(__HAL_GPIO_EXTI_GET_IT(MCP_001_INTB_Pin) != RESET){
	  __HAL_GPIO_EXTI_CLEAR_IT(MCP_001_INTB_Pin);
	  GPIO_EXTI10_Callback();
  }
  else if(__HAL_GPIO_EXTI_GET_IT(MCP_000_INTB_Pin) != RESET){
	  __HAL_GPIO_EXTI_CLEAR_IT(MCP_000_INTB_Pin);
	  GPIO_EXTI11_Callback();
  }
  else if(__HAL_GPIO_EXTI_GET_IT(MCP_000_INTA_Pin) != RESET){
	  __HAL_GPIO_EXTI_CLEAR_IT(MCP_000_INTA_Pin);
	  GPIO_EXTI12_Callback();
  }
  else if(__HAL_GPIO_EXTI_GET_IT(MCP_101_INTA_Pin) != RESET){
	  __HAL_GPIO_EXTI_CLEAR_IT(MCP_101_INTA_Pin);
	  GPIO_EXTI13_Callback();
  }
  else if(__HAL_GPIO_EXTI_GET_IT(MCP_011_INTA_Pin) != RESET){
	  __HAL_GPIO_EXTI_CLEAR_IT(MCP_011_INTA_Pin);
	  GPIO_EXTI14_Callback();
  }
  else if(__HAL_GPIO_EXTI_GET_IT(MCP_100_INTA_Pin) != RESET){
	  __HAL_GPIO_EXTI_CLEAR_IT(MCP_100_INTA_Pin);
	  GPIO_EXTI15_Callback();
  }

  /* USER CODE END EXTI15_10_IRQn 0 */
  /*HAL_GPIO_EXTI_IRQHandler(MCP_001_INTB_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_000_INTB_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_000_INTA_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_101_INTA_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_011_INTA_Pin);
  HAL_GPIO_EXTI_IRQHandler(MCP_100_INTA_Pin);*/
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */

  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */


  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
