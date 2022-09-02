/*
 * stlink_rx.c
 *
 *  Created on: May 3, 2020
 *      Author: sydxrey
 */


/* Includes ------------------------------------------------------------------*/

#include "stlink_rx_midi.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "midi.h"

/* External variables --------------------------------------------------------*/

extern UART_HandleTypeDef *huart_STlink;


/* Variables ---------------------------------------------------------*/

/* Function prototypes -----------------------------------------------*/

/* User code ---------------------------------------------------------*/

void stlink_Rx_Init(){

	//HAL_UART_Receive_IT(huart_STlink, rxUartSTlinkBuff, 3); // starts listening to incoming message over ST-link USB virtual com port

	/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	//SET_BIT(huart_STlink->Instance->CR3, USART_CR3_EIE);

	/* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
	//SET_BIT(huart_STlink->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
	SET_BIT(huart_STlink->Instance->CR1, USART_CR1_RXNEIE);

}

void stlink_Rx_IRQ_Handler(){

	/* TODO L432 vers F446 :

	uint16_t  data = (uint16_t) READ_REG(huart_STlink->Instance->RDR);
	//printf("%d\n", data);
	midi_Process_Byte(data);

	 */
}
















