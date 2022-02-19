/*
 * print.cpp
 *
 *  Created on: Nov 18, 2020
 *      Author: sydxrey
 */


#include "print.h"
#include "string.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"

#define USE_PRINTSERIAL

extern UART_HandleTypeDef huart1;

char print_buffer[50];
//uint8_t midi_buffer[3];

void printSerial(){
#ifdef USE_PRINTSERIAL
	HAL_UART_Transmit(&huart1, (uint8_t *)print_buffer , strlen(print_buffer), 100);
#endif
}

/*void sendMidiMessage(uint8_t status, uint8_t cc, uint8_t val){
#ifndef USE_PRINTSERIAL
	midi_buffer[0] = status;
	midi_buffer[1] = cc;
	midi_buffer[2] = val;
	HAL_UART_Transmit(&huart2, (uint8_t *)midi_buffer , 3, 100);
#endif
}*/

