/*
 * dac_board.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */


/* Includes ------------------------------------------------------------------*/

#include "stm32l4xx_hal.h"
#include "stlink_rx_midi.h"
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

extern MidiNote midi_Note;
extern TIM_HandleTypeDef *htimDac;
extern UART_HandleTypeDef *huart_STlink;


/* Variables ---------------------------------------------------------*/

Boolean is_control_voltages_need_update = FALSE;
int dacTick=0; // adsr enveloppes

uint16_t t; // debug
int debug_counter;

int sw1_Tick, sw2_Tick; // tick in ms to measure delay in order to debounce switches
Boolean is_SW1_IRQ_Pending = FALSE;
Boolean is_SW2_IRQ_Pending = FALSE;

/* Function prototypes -----------------------------------------------*/

static void dac_Board_Timer_incTick();
static void dac_Board_Reset_Devices();
static void dac_Board_Infinite_Loop();
static void dac_Board_Update_Control_Voltages();
static void dac_Board_Init();
static void dac_Board_EXTI_Debouncer();
static void dac_Board_Switch1_Pressed();
static void dac_Board_Switch2_Pressed();
static void dac_Board_Switch1_Released();
static void dac_Board_Switch2_Released();

static void init_Synth_Params();

//static void test_MCP23017();
//static void test_Tim_IRQ();
//static void test_AD5391();

/* User code ---------------------------------------------------------*/



/**
 * Initialize synth parameters and DAC values
 */
static void init_Synth_Params(){

	init_Vca();
	init_Vcf();
	init_Vco();

	for (int channel=0; channel < AD5391_CHANNEL_COUNT; channel++){
		ad5391_Write_Dma(dacTick);
		HAL_Delay(2);
	}
	mcp23017_Tx_GpioA_Buffer_Dma(); HAL_Delay(2);
	mcp23017_Tx_GpioB_Buffer_Dma(); HAL_Delay(2);
}


// --------------------------------------------------------------------------------------------------
//                                     DAC TIMER
// --------------------------------------------------------------------------------------------------

/*
 * starts the htimDac timer - dac_Board_TIM_IRQ callback is called @ 20kHz
 */
void dac_Board_Timer_Start(){

	__HAL_TIM_ENABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(htimDac);

}

/*
 * stops the htimDac timer - dac_Board_TIM_IRQ callback not called anymore
 */
void dac_Board_Timer_Stop(){

	__HAL_TIM_DISABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE(htimDac);

}


// increments dacTick counter
static void dac_Board_Timer_incTick(){
	dacTick++;
	if (dacTick == ADSR_TIMER_PERIOD_FACTOR) {
		dacTick = 0;
	}
}

/**
 * Timer driven DAC and MCP23017 data update callback.
 *
 * Timings :
 * - Called every 50us = 1/20kHz approximately.
 * - At 5Mbits/s, a complete 24 bit transfer takes 5us and then the busy signal goes low for 600ns hence minimum timer period must be above 6us.
 *
 * How this works:
 * - dacTick counter runs from 0 to 20 and indicates what should be updated during each call ; each task below is triggered once every 1ms = 20 * 50us
 *
 * - dackTick=0 : marks envelopes as needing updates (this will be carried out asynchronously in the main while loop)
 * - dackTick=0 to 15 : write AD5391 16-channel data to device using DMA
 * - dackTick=16 : write MCP23017 Port A data to device using DMA
 * - dackTick=17 : write MCP23017 Port B data to device using DMA
 * - dackTick=18 : free for any task
 * - dackTick=19 : free for any task
 *
 * AD5391_CHANNEL_COUNT=16
 * ADSR_TIMER_PERIOD_FACTOR=20
 *
 */
void dac_Board_Timer_IRQ(){

	//LD3_GPIO_Port->BRR = LD3_Pin; // debug

	if (dacTick == 0) is_control_voltages_need_update = TRUE; // TODO : put this at the end, when dacTick > 17?


	if (dacTick < AD5391_CHANNEL_COUNT) ad5391_Write_Dma(dacTick); // 0 to 15
	else if (dacTick == AD5391_CHANNEL_COUNT) mcp23017_Tx_GpioA_Buffer_Dma(); // 16
	else if (dacTick == (AD5391_CHANNEL_COUNT+1)) mcp23017_Tx_GpioB_Buffer_Dma(); // 17

	dac_Board_Timer_incTick();
	//LD3_GPIO_Port->BSRR = LD3_Pin; // debug
}


/*
 * Asynchronously updates all control voltages in turn, e.g., from MIDI input and LFO modulations.
 * This does not write to DAC, just to buffers.
 * Buffers are then written to DAC by dac_Board_Timer_IRQ().
 */
static void dac_Board_Update_Control_Voltages(){

	if (is_control_voltages_need_update==FALSE) return;

	LD3_GPIO_Port->BSRR = LD3_Pin; // debug

	/*t++;
	if (t == 2000) dac_Board_Switch1_Pressed();
	else if (t >= 4000) {
		dac_Board_Switch1_Released();
		t=0;
	}*/

	//uint16_t x = (uint16_t)(2000. * (1.0+sin(0.1 * (debug_t++))));
	//dacWrite(x, DAC_VCA);

	// update functions are called once per ms so as to allow for LFO modulation:

	updateVco13700Freq();
	updateVco3340AFreq();
	updateVco3340BFreq();
	updateVco3340ALevel();
	updateVco3340BPulseLevel();
	updateVco3340BSawLevel();
	updateVco3340BTriLevel();
	updateVco3340APWMDuty();
	updateVco3340BPWMDuty();
	update_Vca_Envelope(); // 15us
	update_Vcf_Envelope(); // 15us
	updateVcfResonance();

	LD3_GPIO_Port->BRR = LD3_Pin; // debug

	is_control_voltages_need_update=FALSE;
}

/*
 * The main infinite loop.
 */
static void dac_Board_Infinite_Loop(){

	while(1){

		dac_Board_Update_Control_Voltages();

		dac_Board_EXTI_Debouncer();

	}
}

// ---------------------------------------------------------------------------------------------------------------
//                                     DEVICES (see also ad5391.c and mcp23017.c for implementation specific details)
// ---------------------------------------------------------------------------------------------------------------

/*
 * Calls what is necessary to init the DAC board
 */
static void dac_Board_Init(){

	dac_Board_Reset_Devices(); // also MCP23017
	ad5391_Init_Device();
	mcp23017_Init_Device();

	init_Adsr_Parameters();
	init_Synth_Params();

	sw1_Tick = HAL_GetTick();
	sw2_Tick = HAL_GetTick();

}

void dac_Board_Start(){

	dac_Board_Init();

	stlink_Rx_Init();

	dac_Board_Timer_Start();
	dac_Board_Infinite_Loop();


}

/**
 * Sends a negative RESET pulse to the AD5391 and MCP23017 circuits.
 */
static void dac_Board_Reset_Devices(){

	DAC_RST_GPIO_Port->BRR = (uint32_t)(DAC_RST_Pin); // lower RST
	HAL_Delay(1); // wait at leat 270us
	DAC_RST_GPIO_Port->BSRR = (uint32_t)DAC_RST_Pin; // raise RST again
	HAL_Delay(1); // necessary?

}


// ---------------------------------------------------------------------------------------------------------------
//                                     SWITCHES SW1 and SW2
// ---------------------------------------------------------------------------------------------------------------

void dac_Board_EXTI_IRQHandler_SW1(){ // EXTI9_5_IRQHandler

	sw1_Tick = HAL_GetTick();
	is_SW1_IRQ_Pending = TRUE;

}

void dac_Board_EXTI_IRQHandler_SW2(){ // EXTI15_10_IRQHandler

	sw2_Tick = HAL_GetTick();
	is_SW2_IRQ_Pending = TRUE;

}

/*
 * Post-delay EXTI debouncer that dispatches to appropriate dac_Board_SwitchX_YYYY().
 *
 * The point is to read the value of the SW1/SW2 GPIO once they've stabilized, then to
 * re-enable interrupts.
 */
static void dac_Board_EXTI_Debouncer(){

	if (is_SW1_IRQ_Pending && (HAL_GetTick() - sw1_Tick > SWITCH_DEBOUNCE_DELAY)){

		is_SW1_IRQ_Pending = FALSE;

		if ((EXTI5_SW1_GPIO_Port->IDR & EXTI5_SW1_Pin) != 0)
			dac_Board_Switch1_Released();
		else
			dac_Board_Switch1_Pressed();

		// before re-enabling IRQs, make sure there's no more IRQ pending:
		while (HAL_NVIC_GetPendingIRQ(EXTI9_5_IRQn)) {
			__HAL_GPIO_EXTI_CLEAR_IT(EXTI5_SW1_Pin);
			HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
		}

		NVIC_EnableIRQ(EXTI9_5_IRQn);   // SW1 = EXTI5
	}

	if (is_SW2_IRQ_Pending && (HAL_GetTick() - sw2_Tick > SWITCH_DEBOUNCE_DELAY)){

		is_SW2_IRQ_Pending = FALSE;

		if ((EXTI11_SW2_GPIO_Port->IDR & EXTI11_SW2_Pin) != 0)
			dac_Board_Switch2_Released();
		else
			dac_Board_Switch2_Pressed();

		// before re-enabling IRQs, make sure there's no more IRQ pending:
		while (HAL_NVIC_GetPendingIRQ(EXTI15_10_IRQn)) {
			__HAL_GPIO_EXTI_CLEAR_IT(EXTI11_SW2_Pin);
			HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		}

		// debug printf("ITfl=%d pend=%d\n", __HAL_GPIO_EXTI_GET_IT(EXTI11_SW2_Pin), HAL_NVIC_GetPendingIRQ(EXTI15_10_IRQn));

		NVIC_EnableIRQ(EXTI15_10_IRQn); // SW2 = EXTI11
	}





}

/**
 * Called when SW1 gets pressed (SW1 is the rightmost button)
 */
static void dac_Board_Switch1_Pressed(){

	printf("SW1 pressed %d\n", debug_counter++);

	// note ON
	//int randomNote = rand() % 20 + 34;
	//int randomNote = 30;
	//printf("rd note=%d\n", randomNote);
	midi_Process_Incoming_Message(NOTE_ON, 30,100);

	//debug_counter++;
}

/**
 * Called when SW1 gets released (SW1 is the rightmost button)
 */
static void dac_Board_Switch1_Released(){

	printf("SW1 released %d\n", debug_counter++);

	midi_Process_Incoming_Message(NOTE_OFF, 30, 0);

}


/**
 * Called when SW2 gets pressed (SW2 is the leftmost button)
 */
static void dac_Board_Switch2_Pressed(){

	printf("SW2 pressed %d\n", debug_counter++);

}

/**
 * Called when SW2 gets released (SW2 is the leftmost button)
 */
static void dac_Board_Switch2_Released(){

	printf("SW2 released %d\n", debug_counter++);

}

// ---------------------------------------------------------------------------------------------------------------
//                                     TEST code
// ---------------------------------------------------------------------------------------------------------------


/*void test_Dac_Board(){

	//test_MCP23017();
	test_Tim_IRQ();
	//test_AD5391();

}*/

/*static void test_AD5391(){

	int t=0;
	uint16_t x;

	dac_Board_Reset_Devices();

	ad5391_Init_Device();

  	//LD3_GPIO_Port->BRR = LD3_Pin; // debug
  	//LD3_GPIO_Port->BSRR = LD3_Pin; // debug


	while (1){

		x = (uint16_t)(2000. * (1.0+sin(0.1 * (t++))));
		dacWrite_Blocking(x, 0);
		HAL_Delay(1);
	}
}*/


/*
 * Test code for the MCP23017 device.
 */
/*static void test_MCP23017(){

	dac_Board_Reset_Devices();

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
}*/

/*static void test_Tim_IRQ(){

	//int t=0;
	//uint16_t x;
	//debug_counter = 0;


	dac_Board_Reset_Devices();

	ad5391_Init_Device();

	mcp23017_Init_Device();

	dac_Board_Timer_Start();

	dac_Board_Infinite_Loop();

}*/


