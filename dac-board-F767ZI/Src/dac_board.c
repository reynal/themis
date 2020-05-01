/*
 * dac_board.c
 *
 *  Created on: Oct 2, 2018
 *      Author: reynal
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32f7xx_hal.h"
#include "midi.h"
#include "mcp4822.h"
#include "main.h"
#include "adsr.h"
#include "vco.h"
#include "vcf.h"
#include "vca.h"
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"

/* External variables --------------------------------------------------------*/

extern MidiNote midiNote;
extern TIM_HandleTypeDef *htimDacs;



/* Variables ---------------------------------------------------------*/


// ------------- for debugging purpose ---------
//int dbg_modulation = 0; // temporaire pour Salomé :) Dec 3 2019
//int dbg_modulation_counter = 0;

// ------------ adsr enveloppes -----------
int dacTick=0;

// DAC write tasks to be executed in turn at every call of adsrTIMCallback():
dacTIMUpdateTask dacTIMUpdateTaskList[ADSR_TIMER_PERIOD_FACTOR] = {
		updateVco13700Freq, // ALWAYS update VCO *before* VCA so that we won't hear the note jump
		updateVco3340AFreq,
		updateVco3340BFreq,
		updateVcaEnvelope,
		updateVcfEnvelope,
		updateVcfResonance,
		updateVco3340ALevel,
		updateVco3340BPulseLevel,
		updateVco3340BSawLevel,
		updateVco3340BTriLevel,
		updateVco3340APWMDuty,
		updateVco3340BPWMDuty,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
};

/* Function prototypes -----------------------------------------------*/

static void incDacTick();


/* User code ---------------------------------------------------------*/

void switchRedLED(GPIO_PinState state){
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, state);
}

void switchRedLEDOn(){
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
}

void switchRedLEDOff(){
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
}

void toggleRedLED(){
	HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
}


void switchBlueLED(GPIO_PinState state){
	HAL_GPIO_WritePin(GPIOB, LD2_Pin, state);
}

void switchBlueLEDOn(){
	HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
}

void switchBlueLEDOff(){
	HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
}

void toggleBlueLED(){
	HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
}


// test every GPIO (output) in turn
void testGPIOVcfVco(){

	while(1){
		HAL_GPIO_TogglePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin);		// CN11#57 (PD0)
		//HAL_GPIO_TogglePin(TRI_3340_GPIO_Port, TRI_3340_Pin);			// CN11#62 (PE6)
		//HAL_GPIO_TogglePin(SAW_3340_GPIO_Port, SAW_3340_Pin);			// CN11#63 (PG9)
		//HAL_GPIO_TogglePin(SYNC_3340_GPIO_Port, SYNC_3340_Pin);		// CN11#65 (PG12)

		HAL_GPIO_TogglePin(DRUM_KICK_GPIO_Port, DRUM_KICK_Pin);			// CN11#50 (PE5)
		HAL_GPIO_TogglePin(DRUM_SNARE_GPIO_Port, DRUM_SNARE_Pin); 		// CN11#55 (PD1) (not soldered yet)
		HAL_GPIO_TogglePin(DRUM_RIM_GPIO_Port, DRUM_RIM_Pin);			// CN11#53 (PF0)
		HAL_GPIO_TogglePin(DRUM_LOWTOM_GPIO_Port, DRUM_LOWTOM_Pin);		// CN11#51 (PF1)
		HAL_GPIO_TogglePin(DRUM_HIGHTOM_GPIO_Port, DRUM_HIGHTOM_Pin);	// CN11#52 (PF2)

		HAL_GPIO_TogglePin(VCF_2NDORDER_GPIO_Port, VCF_2NDORDER_Pin);	// CN11#48 (PE4)
		HAL_GPIO_TogglePin(VCF_4THORDER_GPIO_Port, VCF_4THORDER_Pin);	// CN11#46 (PE2)

		HAL_Delay(50);
		HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
	}
}


// --------------------------------------------------------------------------------------------------
//                                     DAC TIMER
// --------------------------------------------------------------------------------------------------

void startDacTIM() {

	HAL_TIM_Base_Start_IT(htimDacs);

}

void stopDacTIM() {

	HAL_TIM_Base_Stop_IT(htimDacs);

}

/**
 * Timer driven DAC update callback.
 *
 * Called every 50us = 1/20kHz approximately.
 *
 * - every 50us we push a new sample to the DIG VCO dac (sample freq = 20kHz)
 *
 * - every 1ms = 20 * 50us (i.e. every 20 calls) we push a new sample to every other envelopes DAC in turn (1kHz sample freq)
 * BUT since after each SPI bus write we must wait (around 16 * 1/3MHz = 5us) for the transfer to complete before writing a new word,
 * it'd be a waste of time... so there's a clever trick that consists in writing one distinct enveloppe at each timer call
 * (since there are 20 timer calls b/w every envelope update, we could update up to 20 distinct enveloppes this way!
 * of course there are only 15 available DAC's on the board, so we do nothing during the last 5 calls anyway)
 */
void dacTIMCallback(){

		// white noise for the drum machine :
		/* debug 2/4/19 int noise = (int)(4096.0 * rand() / RAND_MAX);
		dacWrite(noise, DAC_NOISE);*/

		dacTIMUpdateTask f = dacTIMUpdateTaskList[dacTick];
		if (f != NULL) f();
		incDacTick();



		/*switch (adsrTick){
			case 0:
				updateVco13700Freq();
				break;
			case 1:
				updateVco3340AFreq();
				break;
			case 2:
				updateVco3340BFreq();
				break;
			case 3:
				updateVCAEnvelope();
				break;
			case 4:
				updateVCFEnvelope();
				break;
			case 5: // LFO
				// dbg_modulation = (int)(400 * sin(10.0 * 6.28e-3 * (dbg_modulation_counter++))); // sin (2 Pi n 1ms)
				break;

			// todo : mixers modulation, etc
		}*/


}

static void incDacTick(){
	dacTick++;
	if (dacTick == ADSR_TIMER_PERIOD_FACTOR) {
		dacTick = 0;
	}
}


/**
 * Initialize synth parameters and DAC values
 */
void initSynthParams(){

	// wavetable init:
	//int i;
	//for (i=0; i<WAVE_TABLE_LEN; i++) waveTable[i] = 1000 * (1.0 + sin(0.0628 * i));

	dacVcaWrite(0.0); // makes sure we don't hear anything
	HAL_Delay(1); // wait 1ms for transfer to complete (could be lower but HAL_Delay can't go below)

	initVcf();
	initVco();

}

/**
 * Called when the blue button gets pressed or released.
 */
void dac_Board_Switch1_ActionPerformed(GPIO_PinState  state){

	if (state == GPIO_PIN_SET){ // note ON
		int randomNote = rand() % 20 + 34;
		printf("rd note=%d\n", randomNote);
		processIncomingMidiMessage(NOTE_ON, randomNote,100);
	}
	else { // note OFF
		processIncomingMidiMessage(NOTE_OFF, 40, 0);
	}
}

