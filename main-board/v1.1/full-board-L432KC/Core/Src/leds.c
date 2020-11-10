/*
 * leds.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#include "main.h"

/* User code ---------------------------------------------------------*/

void switchGreenLED(GPIO_PinState PinState){
	if(PinState != GPIO_PIN_RESET)
		LD3_GPIO_Port->BSRR = (uint32_t)LD3_Pin;
	else
		LD3_GPIO_Port->BRR = (uint32_t)LD3_Pin;
}

void switchGreenLEDOn(){
	LD3_GPIO_Port->BSRR = (uint32_t)LD3_Pin;
}

void switchGreenLEDOff(){
	LD3_GPIO_Port->BRR = (uint32_t)LD3_Pin;
}

void toggleGreenLED(){
	if ((LD3_GPIO_Port->ODR & LD3_Pin) != 0x00u)
		LD3_GPIO_Port->BRR = (uint32_t)LD3_Pin;
	else
		LD3_GPIO_Port->BSRR = (uint32_t)LD3_Pin;
}


void switchRedLED(GPIO_PinState PinState){
	if(PinState != GPIO_PIN_RESET)
		LD2_GPIO_Port->BSRR = (uint32_t)LD2_Pin;
	else
		LD2_GPIO_Port->BRR = (uint32_t)LD2_Pin;
}

void switchRedLEDOn(){
	LD2_GPIO_Port->BSRR = (uint32_t)LD2_Pin;
}

void switchRedLEDOff(){
	LD2_GPIO_Port->BRR = (uint32_t)LD2_Pin;
}

void toggleRedLED(){
	if ((LD2_GPIO_Port->ODR & LD2_Pin) != 0x00u)
		LD2_GPIO_Port->BRR = (uint32_t)LD2_Pin;
	else
		LD2_GPIO_Port->BSRR = (uint32_t)LD2_Pin;
}



