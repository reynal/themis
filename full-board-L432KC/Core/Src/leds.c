/*
 * leds.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#include "main.h"

/* User code ---------------------------------------------------------*/

void switchGreenLED(GPIO_PinState state){
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, state);
}

void switchGreenLEDOn(){
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
}

void switchGreenLEDOff(){
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
}

void toggleGreenLED(){
	HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
}


void switchRedLED(GPIO_PinState state){
	HAL_GPIO_WritePin(GPIOB, LD2_Pin, state);
}

void switchRedLEDOn(){
	HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
}

void switchRedLEDOff(){
	HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
}

void toggleRedLED(){
	HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
}



