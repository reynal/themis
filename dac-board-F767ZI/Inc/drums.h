/*
 * drums.h
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey
 */

#ifndef DRUMS_H_
#define DRUMS_H_

#include "stm32f7xx_hal.h"

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


#endif /* DRUMS_H_ */
