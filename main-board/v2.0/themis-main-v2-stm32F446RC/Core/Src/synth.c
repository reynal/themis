/*
 * synth.c
 *
 * Main file for DAC timer management and CV updating.
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */


/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "ad5644.h"
#include "bh2221.h"
#include "synth.h"
#include "mixer.h"
//#include "midi.h"
#include "stlink_rx_midi.h"
#include "main.h"
#include "adsr.h"
#include "vco.h"
#include "vco_calibration.h"
#include "vcf.h"
#include "vca.h"
//#include "stdio.h"
#include "leds.h"
#include "switches.h"

/* External variables --------------------------------------------------------*/

extern TIM_HandleTypeDef htim1;
#define htimDac (&htim1)


/* Variables ---------------------------------------------------------*/

static bool is_control_voltages_need_update = false;
static int dacTick=0; // adsr enveloppes


/* Function prototypes -----------------------------------------------*/

static void synthInfiniteLoop();
static void synthUpdateControlVoltages();
static void synthTimerPeriodInit();
static void synthParametersInit();

/* User code ---------------------------------------------------------*/



/**
 * Initialize synth parameters and DAC values
 */
static void synthParametersInit(){

	vcaInit();
	vcfInit();
	vcoInit();

	for (int channel=0; channel < AD5644_CHANNEL_COUNT; channel++){
		ad5644FlushBufferDma(dacTick);
		HAL_Delay(2);
	}

	for (int channel=0; channel < BH2221_CHANNEL_COUNT; channel++){
		bh2221FlushBufferDma(dacTick);
		HAL_Delay(2);
	}

}


// --------------------------------------------------------------------------------------------------
//                                     DAC TIMER
// --------------------------------------------------------------------------------------------------

/*
 * Init htimDac period (aka ARR autoreload register) so that this timer IRQ is called at a rate
 * that guarantees that VCA, VCF and LFO are updated ONCE EVERY MS.
 *
 * Note: htmiDac is currently TIM2 which is on APB1 clock. TODO
 */
static void synthTimerPeriodInit(void)
{
	/* Get PCLK frequency for APB1 */
	uint32_t pclk = HAL_RCC_GetPCLK1Freq(); // 45MHz

	/* Get PCLK prescaler (currently we have prescaler equal to 1) */
	//if((RCC->CFGR & RCC_CFGR_PPRE1) != 0) pclk *= 2; // 90MHz

	// 1000 Hz = pclk / (ARR+1) / (PSC+1)
	// whereby ARR = [ pclk / (PSC+1) / 1000 ] - 1

	double frequency = 1000.0 * ADSR_TIMER_FREQUENCY_KHZ * ADSR_TIMER_PERIOD_FACTOR; // 20kHz
	int arr = (uint32_t) ( (double)pclk / (htimDac->Instance->PSC+1) / frequency ) - 1; // 4499
	htimDac->Instance->ARR = arr;

}


/*
 * starts the htimDac timer - dac_Board_TIM_IRQ callback is called @ 20kHz
 */
void synthStartDacTimer(){

	__HAL_TIM_ENABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(htimDac);

}

/*
 * stops the htimDac timer - dac_Board_TIM_IRQ callback not called anymore
 */
void synthStopDacsTimer(){

	__HAL_TIM_DISABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE(htimDac);

}


/**
 * Timer driven DAC and MCP23017 data update callback.
 *
 * Timings :
 * - Called every 50us = 1/20kHz.
 *
 * - dacTick counter runs from 0 to 20 and indicates what should be updated during each call ; each task below is triggered once every 1ms = 20 * 50us
 *
 */
void synthDacTimerIRQ(){

	HAL_GPIO_TogglePin(EXTENSION_1_GPIO_Port, EXTENSION_1_Pin); // debug


	switch (dacTick){
	case 0:
		ad5644FlushBufferDma(AD5644_VCO_3340A_FREQ); break;
	case 1:
		bh2221FlushBufferDma(BH2221_VCO_3340A_PWM_DUTY); break;
	case 2:
		bh2221FlushBufferDma(BH2221_VCO_3340B_PWM_DUTY); break;
	case 3:
		ad5644FlushBufferDma(AD5644_VCO_3340B_FREQ); break;
	case 4:
		bh2221FlushBufferDma(BH2221_V2140D_3340A_PULSE_LVL); break;
	case 5:
		bh2221FlushBufferDma(BH2221_V2140D_3340A_SAW_LVL); break;
	case 6:
		ad5644FlushBufferDma(AD5644_WAVE_GEN); break;
	case 7:
		bh2221FlushBufferDma(BH2221_V2140D_3340A_TRI_LVL); break;
	case 8:
		bh2221FlushBufferDma(BH2221_V2140D_3340B_PULSE_LVL); break;
	case 9:
		ad5644FlushBufferDma(AD5644_VCA); break;
	case 10:
		bh2221FlushBufferDma(BH2221_V2140D_3340B_SAW_LVL); break;
	case 11:
		bh2221FlushBufferDma(BH2221_V2140D_3340B_TRI_LVL); break;
	case 12:
		bh2221FlushBufferDma(BH2221_VCF_CUTOFF); break;
	case 13:
		bh2221FlushBufferDma(BH2221_VCF_RES); break;
	case 14:
		bh2221FlushBufferDma(BH2221_V2140D_XMOD_LVL); break;
	case 15:
		bh2221FlushBufferDma(BH2221_V2140D_3340B_SUB_LVL); break;
	case 16:
		is_control_voltages_need_update = true; break; // now we've time to update CV (called at 1kHz)

		// 16 to 19 are free for any additionnal task
	}

	//ad5644WriteAsync(dacTick * 600, 2);
	//ad5644FlushBufferDma(2);


	dacTick++;
	if (dacTick >= ADSR_TIMER_PERIOD_FACTOR) dacTick = 0;
}


/*
 * Asynchronously updates all control voltages in turn, e.g., from MIDI input and LFO modulations.
 * This does not write to DAC, just to buffers.
 * Buffers are then written to DAC by the timer IRQ Handler.
 */
static void synthUpdateControlVoltages(){

	if (is_control_voltages_need_update==false) return;

	// update functions are called once per ms so as to allow for LFO modulation:

	vcoWrite3340AFreqToDac();
	vcoWrite3340BFreqToDac();

	vcoWrite3340APWMDutyToDac();
	vcoWrite3340BPWMDutyToDac();

	mixerWriteVco3340APulseLevelToDac();
	mixerWriteVco3340ASawLevelToDac();
	mixerWriteVco3340ATriLevelToDac();

	mixerWriteVco3340BPulseLevelToDac();
	mixerWriteVco3340BSawLevelToDac();
	mixerWriteVco3340BTriLevelToDac();

	adsrUpdateVcaEnvelope(); // 15us
	adsrUpdateVcfEnvelope(); // 15us

	vcfWriteResonanceToDac(); // cutoff has been written to DAC from adsrUpdateVcfEnvelope()

	is_control_voltages_need_update=false;
}

/*
 * The main infinite loop.
 */
static void synthInfiniteLoop(){

	while(1){

		synthUpdateControlVoltages();

		switchScanButtonsState();

	}
}

// ---------------------------------------------------------------------------------------------------------------
//                                     DEVICES (see also ad5391.c and mcp23017.c for implementation specific details)
// ---------------------------------------------------------------------------------------------------------------

void synthStart(){

	ledInit();

//	stlink_Tx_dma_init(); // DMA printf !!!

	synthTimerPeriodInit(); // 20kHz : validé 30 aout 2022

	ad5644Init();
	bh2221InitDMA();

	adsrInitParameters();
	synthParametersInit();

	switchInit();

	stlink_Rx_Init(); // MIDI

	synthStartDacTimer();

	synthInfiniteLoop();


}





