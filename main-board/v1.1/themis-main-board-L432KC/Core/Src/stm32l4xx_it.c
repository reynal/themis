/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dac_board.h"
#include "stdio.h"
#include "stlink_rx_midi.h"
#include "stlink_tx_dma.h"
#include "vco_calibration.h"

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
extern DMA_HandleTypeDef hdma_i2c3_tx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

extern I2C_HandleTypeDef *hi2c_MCP23017;
extern DMA_HandleTypeDef *hdma_MCP23017_tx;

extern DMA_HandleTypeDef *hdma_STlink_tx;
extern UART_HandleTypeDef *huart_STlink;

extern DMA_HandleTypeDef *hdma_Dac_tx;

extern TIM_HandleTypeDef *htimVcoCalib;
extern TIM_HandleTypeDef *htimDac;

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
  * @brief This function handles Prefetch fault, memory access fault.
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
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

	// IT not used for I2C, leaving code here for the sake of documentation (SR: April 2020)
	hdma_MCP23017_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF2; // clear interrupt flag
	hi2c_MCP23017->Instance->CR1 &= ~I2C_CR1_TXDMAEN; /* Disable DMA Request */
	//__HAL_I2C_ENABLE_IT(hi2c, I2C_IT_STOPI | I2C_IT_TCI); /* Enable ST

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */

	// this handler not used yet (see ad5391.c, DMA init)
	hdma_Dac_tx ->DmaBaseAddress->IFCR = DMA_ISR_GIF3; // clear interrupt flag

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

	stlink_dma_irq_handler();

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

	// ---- SWITCH 1 ----

	//HAL_GPIO_EXTI_IRQHandler(EXTI5_SW1_Pin);

	__HAL_GPIO_EXTI_CLEAR_IT(EXTI5_SW1_Pin);

	NVIC_DisableIRQ(EXTI9_5_IRQn); // will be re-enabled after some delay to avoid spurious interrupts from switch due to noise

	dac_Board_EXTI_IRQHandler_SW1();

  /* USER CODE END EXTI9_5_IRQn 0 */
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */

	  /* TIM Update event */
	  //if (__HAL_TIM_GET_FLAG(htimVcoCalib, TIM_FLAG_UPDATE) != RESET && __HAL_TIM_GET_IT_SOURCE(htimVcoCalib, TIM_IT_UPDATE) != RESET){
	      __HAL_TIM_CLEAR_IT(htimVcoCalib, TIM_IT_UPDATE);
	      vcoCalib_UP_IRQHandler();
	  //}

  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */

	// Input Capture events on channels 1, 2 and 3 (see vco_calibration.c) :

	// following code was taken from HAL_TIM_IRQHandler() in stm32l4xx_hal_tim.c:
	// see TIM1->DIER register and TIM1->SR status register
	// here we check what capture/compare interrupt flag was set by hardware, from CC1IF, CC2IF and CC3IF:

	if (__HAL_TIM_GET_FLAG(htimVcoCalib, TIM_FLAG_CC1) != RESET) {// && __HAL_TIM_GET_IT_SOURCE(htimVcoCalib, TIM_IT_CC1) != RESET) {
			__HAL_TIM_CLEAR_IT(htimVcoCalib, TIM_IT_CC1);
			//if ((htimVcoCalib->Instance->CCMR1 & TIM_CCMR1_CC1S) != 0x00U) // Capture/Compare 1 Selection is set
				vcoCalib_IC_IRQHandler(TIM_CHANNEL_1);
	}

	if (__HAL_TIM_GET_FLAG(htimVcoCalib, TIM_FLAG_CC2) != RESET) {// && __HAL_TIM_GET_IT_SOURCE(htimVcoCalib, TIM_IT_CC2) != RESET) {
			__HAL_TIM_CLEAR_IT(htimVcoCalib, TIM_IT_CC2);
			//if ((htimVcoCalib->Instance->CCMR1 & TIM_CCMR1_CC2S) != 0x00U)
				vcoCalib_IC_IRQHandler(TIM_CHANNEL_2);
	}

	if (__HAL_TIM_GET_FLAG(htimVcoCalib, TIM_FLAG_CC3) != RESET) {// && __HAL_TIM_GET_IT_SOURCE(htimVcoCalib, TIM_IT_CC3) != RESET) {
			__HAL_TIM_CLEAR_IT(htimVcoCalib, TIM_IT_CC3);
			//if ((htimVcoCalib->Instance->CCMR2 & TIM_CCMR2_CC3S) != 0x00U)
				vcoCalib_IC_IRQHandler(TIM_CHANNEL_3);
	}

  /* USER CODE END TIM1_CC_IRQn 0 */
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */

  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
    __HAL_TIM_CLEAR_IT(htimDac, TIM_IT_UPDATE);
    dac_Board_Timer_IRQ();

  /* USER CODE END TIM2_IRQn 0 */
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

	stlink_Rx_IRQ_Handler();

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

	// ---- SWITCH 2 ----

	__HAL_GPIO_EXTI_CLEAR_IT(EXTI11_SW2_Pin);

	NVIC_DisableIRQ(EXTI15_10_IRQn); // will be re-enabled after some delay to avoid spurious interrupts from switch due to noise

	dac_Board_EXTI_IRQHandler_SW2();


  /* USER CODE END EXTI15_10_IRQn 0 */
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
