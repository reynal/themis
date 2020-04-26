/*
 * dac_board.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */


/* Includes ------------------------------------------------------------------*/

#include "stm32l4xx_hal.h"
#include "mcp23017.h"
#include "ad5391.h"
#include "midi.h"
#include "main.h"
#include "adsr.h"
#include "vco.h"
#include "vcf.h"
#include "vca.h"
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "leds.h"
#include "math.h"

/* External variables --------------------------------------------------------*/

extern MidiNote midiNote;
extern TIM_HandleTypeDef *htimDac;


/* Variables ---------------------------------------------------------*/

Boolean is_enveloppes_need_update = FALSE;


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
static void reset_devices();
static void test_MCP23017();
static void test_Tim_IRQ();

/* User code ---------------------------------------------------------*/



/**
 * Initialize synth parameters and DAC values
 */
void initSynthParams(){

	// TODO L4 dacVcaWrite(0.0); // makes sure we don't hear anything
	HAL_Delay(1); // wait 1ms for transfer to complete (could be lower but HAL_Delay can't go below)

	initVcf();
	initVco();

}


// --------------------------------------------------------------------------------------------------
//                                     DAC TIMER
// --------------------------------------------------------------------------------------------------

void start_DAC_Timer(){

	// init TIMER DAC:
	__HAL_TIM_ENABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(htimDac);

}

void stop_DAC_Timer(){

	// init TIMER DAC:
	__HAL_TIM_ENABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(htimDac);

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

		//dacTIMUpdateTask f = dacTIMUpdateTaskList[dacTick];
		//if (f != NULL) f();
		//incDacTick();
}

static void incDacTick(){
	dacTick++;
	if (dacTick == ADSR_TIMER_PERIOD_FACTOR) {
		dacTick = 0;
	}
}

// ---------------------------------------------------------------------------------------------------------------
//                                     DEVICES (see also ad5391.c and mcp23017.c for specific implementations)
// ---------------------------------------------------------------------------------------------------------------

/*
 * Calls what is necessary to init the DAC board
 */
void dac_Board_Init(){

	reset_devices(); // also MCP23017

	ad5391_Init_Device();

	mcp23017_Init_Device();

}

/**
 * Sends a negative RESET pulse to the AD5391 and MCP23017 circuits.
 */
static void reset_devices(){

	DAC_RST_GPIO_Port->BRR = (uint32_t)(DAC_RST_Pin); // lower RST
	HAL_Delay(1); // wait at leat 270us
	DAC_RST_GPIO_Port->BSRR = (uint32_t)DAC_RST_Pin; // raise RST again
	HAL_Delay(1); // necessary?

}



/**
 * DAC IRQ handler that's fit for any timer.
 * Beware: at 5Mbits/s, a complete 24 bit transfer takes 5us and then
 * the busy signal goes low for 600ns hence minimum timer period must be above 6us.
 *
 */
void dac_Board_TIM_IRQ(){

	LD3_GPIO_Port->BRR = LD3_Pin; // debug

	if (dacTick == 0) is_enveloppes_need_update = TRUE;

	if (dacTick < AD5391_CHANNEL_COUNT) ad5391_Write_Dma(dacTick);
	else if (dacTick == AD5391_CHANNEL_COUNT) mcp23017_Tx_GpioA_Buffer_Dma();
	else if (dacTick == (AD5391_CHANNEL_COUNT+2)) mcp23017_Tx_GpioB_Buffer_Dma();

	incDacTick();
	LD3_GPIO_Port->BSRR = LD3_Pin; // debug
}



/**
 * Called when the blue button gets pressed or released.
 */
void blueButtonActionPerformedCallback(GPIO_PinState state){

	if (state == GPIO_PIN_SET){ // note ON
		int randomNote = rand() % 20 + 34;
		printf("rd note=%d\n", randomNote);
		processIncomingMidiMessage(NOTE_ON, randomNote,100);
	}
	else { // note OFF
		processIncomingMidiMessage(NOTE_OFF, 40, 0);
	}
}

// ---------------------------------------------------------------------------------------------------------------
//                                     TEST code
// ---------------------------------------------------------------------------------------------------------------


void test_Dac_Board(){

	//test_MCP23017();
	test_Tim_IRQ();

}


/*
 * Test code for the MCP23017 device.
 */
static void test_MCP23017(){

	reset_devices();

	mcp23017_Init_Device();

  	//LD3_GPIO_Port->BRR = LD3_Pin; // debug
  	//LD3_GPIO_Port->BSRR = LD3_Pin; // debug


	while (1){

		mcp23017_Write_Pin(MCP23017_GPIOPORT_A, GPIO_PIN_0, GPIO_PIN_SET);
		mcp23017_Tx_GpioA_Buffer_Dma();
		HAL_Delay(1);

		mcp23017_Write_Pin(MCP23017_GPIOPORT_A, GPIO_PIN_0, GPIO_PIN_RESET);
		mcp23017_Tx_GpioA_Buffer_Dma();
		HAL_Delay(1);
	}
}

static void test_Tim_IRQ(){

	int t=0;
	int x;

	reset_devices();

	ad5391_Init_Device();

	mcp23017_Init_Device();

	start_DAC_Timer();

	while (1){

		if (is_enveloppes_need_update == TRUE){
			x = (uint32_t)(2000. * (1.0+sin(0.1 * (t++))));
			dacWrite(x, 0);
			is_enveloppes_need_update = FALSE;
		}
		//HAL_Delay(1);
	}
}


