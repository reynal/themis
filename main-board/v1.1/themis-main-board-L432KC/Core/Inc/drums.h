/*
 * drums.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_DRUMS_H_
#define INC_DRUMS_H_

#include "stm32l4xx_hal.h"

//Channels
#define DRUM_CHANNEL 10

/**
 *  struct for drums
 */
typedef struct {
	int bassdrumCounter;
	int rimshotCounter;
	int snareCounter;
	int lowtomCounter;
	int hightomCounter;
} DrumTriggers;

#define BASS_DRUM_NOTE 36
#define RIMSHOT_NOTE 37
#define SNARE_NOTE 38
#define LOWTOM_NOTE 41
#define HIGHTOM_NOTE 48



void playDrumMachine(uint8_t n, uint8_t v);
void updateDrumMachine();


#endif /* INC_DRUMS_H_ */
