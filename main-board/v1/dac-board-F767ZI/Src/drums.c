/*
 * drums.c
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey
 */

/* Includes ------------------------------------------------------------------*/

#include "drums.h"
#include "stm32f7xx_hal.h"
#include "main.h"

/* External variables --------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/

DrumTriggers drumTriggers = { .bassdrumCounter=0, .rimshotCounter=0, .snareCounter=0, .lowtomCounter=0, .hightomCounter=0};

/* Function prototypes -----------------------------------------------*/


/**
 * Triggers a GPIO output to play a drum sound
 * @param data1, data2, last two bytes of midi message
 */

void playDrumMachine(uint8_t note, uint8_t velocity){

	switch(note){
	case BASS_DRUM_NOTE:
		HAL_GPIO_WritePin(DRUM_KICK_GPIO_Port, DRUM_KICK_Pin, GPIO_PIN_RESET);
		drumTriggers.bassdrumCounter = 1;
		break;
	case RIMSHOT_NOTE:
		HAL_GPIO_WritePin(DRUM_RIM_GPIO_Port, DRUM_RIM_Pin, GPIO_PIN_RESET);
		drumTriggers.rimshotCounter = 1;
		break;
	case SNARE_NOTE:
		HAL_GPIO_WritePin(DRUM_SNARE_GPIO_Port, DRUM_SNARE_Pin, GPIO_PIN_SET);
		drumTriggers.snareCounter = 1;
		break;
	case LOWTOM_NOTE:
		HAL_GPIO_WritePin(DRUM_LOWTOM_GPIO_Port, DRUM_LOWTOM_Pin, GPIO_PIN_RESET);
		drumTriggers.lowtomCounter= 1;
		break;
	case HIGHTOM_NOTE:
		HAL_GPIO_WritePin(DRUM_HIGHTOM_GPIO_Port, DRUM_HIGHTOM_Pin, GPIO_PIN_RESET);
		drumTriggers.hightomCounter = 1;
		break;
	}
}



/**
 * called by timer to update trigger signal for drums
 */
void updateDrumMachine(){

	if (drumTriggers.bassdrumCounter > 0) {
		drumTriggers.bassdrumCounter --;
		if(drumTriggers.bassdrumCounter ==0) HAL_GPIO_WritePin(DRUM_KICK_GPIO_Port, DRUM_KICK_Pin, GPIO_PIN_SET);
	}
	if (drumTriggers.rimshotCounter > 0) {
		drumTriggers.rimshotCounter --;
		if(drumTriggers.rimshotCounter ==0) HAL_GPIO_WritePin(DRUM_RIM_GPIO_Port, DRUM_RIM_Pin, GPIO_PIN_SET);
	}
	if (drumTriggers.snareCounter > 0) {
		drumTriggers.snareCounter --;
		if(drumTriggers.snareCounter ==0) HAL_GPIO_WritePin(DRUM_SNARE_GPIO_Port, DRUM_SNARE_Pin, GPIO_PIN_RESET);
	}
	if (drumTriggers.lowtomCounter> 0) {
		drumTriggers.lowtomCounter--;
		if(drumTriggers.lowtomCounter==0) HAL_GPIO_WritePin(DRUM_LOWTOM_GPIO_Port, DRUM_LOWTOM_Pin, GPIO_PIN_SET);
	}
	if (drumTriggers.hightomCounter > 0) {
		drumTriggers.hightomCounter --;
		if(drumTriggers.hightomCounter ==0) HAL_GPIO_WritePin(DRUM_HIGHTOM_GPIO_Port, DRUM_HIGHTOM_Pin, GPIO_PIN_SET);
	}
}
