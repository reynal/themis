/*
 * stlink_rx.c
 *
 *  Created on: May 3, 2020
 *      Author: sydxrey
 */


/* Includes ------------------------------------------------------------------*/

#include "stm32l4xx_hal.h"
#include "stdio.h"
#include "midi.h"

/* External variables --------------------------------------------------------*/

extern UART_HandleTypeDef *huart_STlink;


/* Variables ---------------------------------------------------------*/

uint8_t rxUartSTlinkBuff[3]; // RX BUFF for UART coming from host PC (three MIDI bytes)


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

/**
 * Callback for the UART peripheral receive data process
 * Called when a given amount of data has been received on given UART port
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart == huart_STlink){ // TODO: validate
		printf("Received: %s\n", rxUartSTlinkBuff);
		//toggleBlueLED();
		//processIncomingMidiMessage(rxUartSTlinkBuff[0], rxUartSTlinkBuff[1], rxUartSTlinkBuff[2]);
		HAL_UART_Receive_IT(huart_STlink, rxUartSTlinkBuff, 3); // wait for next MIDI msg
	}
}


void stlink_Rx_IRQ_Handler(){

	// cf. HAL_UART_IRQHandler() in stm32l4xx_hal_uart.c

	//uint32_t isrflags   = READ_REG(huart->Instance->ISR);
	//uint32_t cr1its     = READ_REG(huart->Instance->CR1);
	//uint32_t cr3its     = READ_REG(huart->Instance->CR3);

	//uint32_t errorflags;
	//uint32_t errorcode;

	//errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));

	/*if (((isrflags & USART_ISR_RXNE) != 0U) && ((cr1its & USART_CR1_RXNEIE) != 0U)){
		huart->RxISR(huart);
	}*/

	uint16_t  data = (uint16_t) READ_REG(huart_STlink->Instance->RDR);
	//printf("%d\n", data);
	process_Midi_Byte(data);

	// et lorsque c'est terminé :
	/* Disable the UART Parity Error Interrupt and RXNE interrupts */
	//CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
	/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	//CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);





}


















