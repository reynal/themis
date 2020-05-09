/*
 * vco_calibration.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 *
 * Features:
 * - the lowest note on the piano is a 27.5Hz A.
 * - C-1 		~8.18Hz		#0
 * - A-1		13.75Hz		#9
 * - A0 		27.5Hz		#21
 * - E1			~41.2Hz		#28		(bass guitar 1st string)
 * - A1 		55Hz		#33		(bass guitar 2nd string)
 * - A2 		110Hz		#45		(guitar 2nd string)
 * - A3 		220Hz		#57
 * - C4 		~261.62Hz 	#60 	(middle C)
 * - A4 		440Hz 		#69		ref diapason
 * - A5			880Hz		#81
 * - A6			1760Hz		#93
 * - A7			3520Hz		#105
 * - A8			7040Hz		#117
 * - G9 		~12543Hz	#127
 *
 * VCO 3340 is able to produce a 35Hz as the lowest note (CV at 0mV), and a 500000:1 frequency range typical.
 *
 * Clues about optimal freq range can be found in user guides of commercial synthetizers.
 *
 * For instance, Moog subphatty has OSCILLATOR CALIBRATION RANGE: 22Hz-6.8KHz. Guaranteed note range at 8’ of Note 18 to 116
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32l4xx_hal.h"
#include "tim.h"
#include "main.h"
#include "misc.h"
#include "vco_calibration.h"
#include "vco.h"
#include "ad5391.h"
#include "dac_board.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "leds.h"


/* External variables --------------------------------------------------------*/

extern TIM_HandleTypeDef* htimVcoCalib;
extern TIM_HandleTypeDef *htimVcoCalibSlave;
extern uint32_t  note_To_VCO3340A_CV[128];
extern uint32_t  note_To_VCO3340B_CV[128];
extern uint32_t  note_To_VCO13700_CV[128];

/* variables ---------------------------------------------------------*/

static Vco_Calib vco3340A_calib = {.name="VCO3340A", .note_to_cv=note_To_VCO3340A_CV, .IC_Channel=TIM_CHANNEL_CALIB_VCO3340A, .cv_min=VCO3340A_MIN_INPUT_CV, .cv_max=VCO3340A_MAX_INPUT_CV};
static Vco_Calib vco3340B_calib = {.name="VCO3340B", .note_to_cv=note_To_VCO3340B_CV, .IC_Channel=TIM_CHANNEL_CALIB_VCO3340B, .cv_min=VCO3340B_MIN_INPUT_CV, .cv_max=VCO3340B_MAX_INPUT_CV};
static Vco_Calib vco13700_calib = {.name="VCO13700", .note_to_cv=note_To_VCO13700_CV, .IC_Channel=TIM_CHANNEL_CALIB_VCO13700, .cv_min=VCO13700_MIN_INPUT_CV, .cv_max=VCO13700_MAX_INPUT_CV};
static Vco_Calib vco_Calib_Array[3];

static double time_To_Timer; // convert from a period value (in s) to a timer counting interval TODO : const ?
static uint32_t note_To_Timer[128];

static Boolean is_calib_underway = FALSE; // prevent re-entrance

/*static struct {
	int counter;
	uint32_t values[10];
	uint32_t tim1Cnt[10];
	uint32_t tim15Cnt[10];
	uint32_t deltas[10];
	double freqs[10];
	int timUpdate;
} debug_calib;*/

/* function prototypes -----------------------------------------------*/

static void print_Note_To_CV_Tables();
static void init_Note_To_Timer_Table();
static void init_Calib();
static void start_Calib();

/* user code -----------------------------------------------*/

/**
 * init fields in struct Vco_Calib that need to be initialized before every calibration.
 */
static void init_Calib(){

	vco_Calib_Array[0] = vco3340A_calib;
	vco_Calib_Array[1] = vco3340B_calib;
	vco_Calib_Array[2] = vco13700_calib;

	for (int i=0; i<VCO_COUNT; i++){

		vco_Calib_Array[i].note = -1;
		vco_Calib_Array[i].previous_capture = -1;
		vco_Calib_Array[i].current_interval = 0;
		vco_Calib_Array[i].cv = vco_Calib_Array[i].cv_min;
		for (int midiNote = 0; midiNote < 128 ; midiNote++) vco_Calib_Array[i].note_to_cv[midiNote] = 0;
		dacWrite_Blocking(vco_Calib_Array[i].cv , vco_Calib_Array[i].dac);

	}
}

static void start_Calib(){

	printf("Starting calibration for VCO: ");
	for (int i=0; i<VCO_COUNT; i++){
		HAL_TIM_IC_Start_IT(htimVcoCalib, vco_Calib_Array[i].IC_Channel);
		printf("%s ", vco_Calib_Array[i].name);
	}
	printf("\n");
}

/*
 * Init a table that maps every MIDI note to the corresponding counting interval of the capture timer,
 * based on the equal tempered scale.
 *
 * Also init "time_To_Timer" that converts from a period in seconds
 * to a timer count (aka timer interval), i.e.,
 *
 *       timer_count = time_To_Timer * signal_period
 *
 *       signal_period = timer_count / time_To_Timer
 *
 * and
 *
 *       signal_frequency = time_To_Timer / timer_count
 *
 * For example, midiNote = 69 => f = 440Hz => T = 1/440 => counting_interval = T * time_To_Timer
 *
 * Rk: conversion function would take around 130us ("pow" with double args takes some time...) so on STM32L4 we have to use a table...
 *
 */
static void init_Note_To_Timer_Table(){

	  /* Get PCLK2 frequency for TIM1 */
	  double pclk = HAL_RCC_GetPCLK2Freq(); // 80MHz

	  /* Get PCLK prescaler */
	  if((RCC->CFGR & RCC_CFGR_PPRE2) != 0) pclk *= 2;

	  int tim_icpsc_div = 8; // see tim.c : sConfigIC.ICPrescaler = TIM_ICPSC_DIV1; => input edges trigger timer every tim_icpsc_div events => resulting frequency measurement must be multiplied by tim_icpsc_div

	  time_To_Timer = pclk * tim_icpsc_div / (htimVcoCalib->Instance->PSC + 1);

	  for (int midiNote=0; midiNote<128; midiNote++){
		  note_To_Timer[midiNote] = (int)round(time_To_Timer * A4_PERIOD * pow(2.0, (A4_MIDI_NOTE - midiNote)/12.0));
	  }

}

/**
 * Run a complete calibration process for all VCO in parallel.
 */
void vcoCalib_Run(){

	if (is_calib_underway == TRUE) return; // prevent re-entrance

	is_calib_underway = TRUE;

	dac_Board_Timer_Stop(); // stop timer responsible for updating ADSR enveloppes
	HAL_Delay(10); // wait until current DMA transfer are finished

	vco_Prepare_For_Calibration(); // beware: blocking calls on SPI and I2C! Make sure no DMA still underway!

	init_Note_To_Timer_Table();
	init_Calib();

	__HAL_TIM_ENABLE(htimVcoCalibSlave);
	__HAL_TIM_SetCounter(htimVcoCalibSlave, 0);

	start_Calib();

	//__HAL_TIM_ENABLE_IT(htimVcoCalib, TIM_IT_UPDATE); // TODO useless now

	// while at least one VCO calibration has not completed yet...
	Boolean completed = FALSE;
	while(completed == FALSE){
		  HAL_Delay(50); // LED blinks faster so we know we're calibrating!
		  toggleRedLED(); // debug L4
		  completed = TRUE;
		  for (int i=0; i<VCO_COUNT; i++){
			  completed &= vco_Calib_Array[i].completed;
		  }
	}

	// proceeds with main() code
	//printf("Calibration terminated!\n");
	print_Note_To_CV_Tables();

	switchRedLEDOff();

	// debug L4 dac_Board_Timer_Start(); // restart timer responsible for updating ADSR enveloppes

	__HAL_TIM_DISABLE(htimVcoCalibSlave);

	is_calib_underway = FALSE;

}


/**
 * Print the midiToVCO????CV[128] arrays to the serial terminal,
 * in a format that can be directly paste into the dac_board.c, e.g.
 *
 * int midiToVCO????CV[128] = {
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0-11
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 12-23
		 0, 6, 64, 119, 176, 232, 288, 344, 399, 455, 508, 565,	 // 24-35
		 621, 678, 735, 791, 848, 903, 960, 1014, 1072, 1127, 1184, 1240,	// 36-47
		 1297, 1355, 1410, 1468, 1523, 1580, 1636, 1692, 1748, 1805, 1861, 1917, // 48-59
		 1975, 2030, 2087, 2143, 2199, 2255, 2312, 2368, 2426, 2483, 2539, 2594, // 60-71
		 2650, 2707, 2762, 2819, 2875, 2933, 2988, 3045, 3101, 3157, 3213, 3269, // 72-83
		 3327, 3382, 3440, 3496, 3551, 3607, 3662, 3719, 3774, 3832, 3888, 3945, // 84-95
		 4001, 4057, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, // 96-107
		 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, // 108-119
		 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
		 };
 *
 */
static void print_Note_To_CV_Tables(){

	printf("\n---- calib data ----\n\n");

	for (int i=0; i<VCO_COUNT; i++){
		printf("int midiTo");
		printf("%s", vco_Calib_Array[i].name);
		printf("CV[128] = {\n");
		for (int midiNote = 0; midiNote < 127 ; midiNote++){
			printf("%lu, ", vco_Calib_Array[i].note_to_cv[midiNote]);
			if (midiNote % 12 ==11 ) printf("\n");
		}
		printf("%lu };\n\n", vco_Calib_Array[i].note_to_cv[127]);
	}
}


/**
 * Input Capture IRQ handler.
 * @param Channel TIM_CHANNEL_CALIB_VCO3340A, TIM_CHANNEL_CALIB_VCO3340B or TIM_CHANNEL_CALIB_VCO13700
 *
void _vcoCalib_IC_IRQHandler(uint32_t IC_Channel){

	//uint32_t delta;
	uint32_t x ,i;

	i = __HAL_TIM_GetCounter(&htim15);
	switch (IC_Channel){
	case TIM_CHANNEL_CALIB_VCO3340A:
		x = htimVcoCalib->Instance->CCR_VCO3340A;
		break;
	case TIM_CHANNEL_CALIB_VCO3340B:
		x = htimVcoCalib->Instance->CCR_VCO3340B;
		break;
	case TIM_CHANNEL_CALIB_VCO13700:
		x = htimVcoCalib->Instance->CCR_VCO13700;
		break;
	default:
		break;
	}
	//dbg_Tim1Cnt[dbg_Counter]=x;
	x += i * (htimVcoCalib->Instance->ARR+1);

	//dbg_Values[dbg_Counter] = x;
	//dbg_Tim15Cnt[dbg_Counter]= i;
	/f (dbg_Counter>0) {
		delta = x - dbg_Values[dbg_Counter-1];
		//while (delta<0) delta += htimVcoCalib->Instance->ARR+1;
		dbg_Deltas[dbg_Counter] = delta;
		dbg_Freqs[dbg_Counter] = time_To_Timer_Count_Conversion_Factor / delta;
	}
	dbg_Counter++;
	if (dbg_Counter == 10) {
		printf("%d\n", )
		dbg_Counter=0;
		//__HAL_TIM_SetCounter(&htim15, 0);
	}
}*/

void vcoCalib_UP_IRQHandler(){

	//toggleGreenLED();
	//dbg_TimUpdate++;
}

/**
 * interrupt callback that gets triggered whenever a rising edge is detected on the current VCO calibration pin
 * the global var calibrationTimerInterval is updated after each call
 */
void vcoCalib_IC_IRQHandler(uint32_t IC_Channel){

	// compute timer interval b/w subsequent edges for input signal and store the result in global var "calibrationTimerInterval":

	uint32_t cnt, cntMSW;
	Vco_Calib* vco;

	cntMSW = __HAL_TIM_GetCounter(&htim15);
	switch (IC_Channel){
	case TIM_CHANNEL_CALIB_VCO3340A:
		cnt = htimVcoCalib->Instance->CCR_VCO3340A;
		vco = &vco3340A_calib;
		break;
	case TIM_CHANNEL_CALIB_VCO3340B:
		cnt = htimVcoCalib->Instance->CCR_VCO3340B;
		vco = &vco3340B_calib;
		break;
	case TIM_CHANNEL_CALIB_VCO13700:
		cnt = htimVcoCalib->Instance->CCR_VCO13700;
		vco = &vco13700_calib;
		break;
	default:
		break;
	}
	cnt += cntMSW * (htimVcoCalib->Instance->ARR+1);


	if (vco->previous_capture == -1) { // initial step
		vco->previous_capture = cnt;
		return;
	}
	// now vco->previous_capture is ok

	vco->previous_interval = vco->current_interval;
	vco->current_interval = cnt - vco->previous_capture;
	vco->previous_capture = cnt;

	// compute initial (lowest) note: we assume that the DAC level is at its minimum, so that the VCO outputs a signal with
	// the largest possible period and then we'll look for the closest note which we'll be the one we start the calibration process with:
	if (vco->note == -1){ // not init'd yet
		vco->note = 0;
		// increment note from 0 until the corresponding timer interval goes below the period of the current VCO signal:
		while (vco->note < 128 && note_To_Timer[vco->note] > vco->current_interval){
			vco->note++;
		}

	}

	// look for next DAC level for which the VCO signal has the closest possible period to the current note:
	if (vco->current_interval > note_To_Timer[vco->note]) {
		vco->cv++; // try to decrease signal period until there's a match
		if (vco->cv <= vco->cv_max) { // else see below
			dacWrite_Blocking(vco->cv, vco->dac);
		}
	}
	else { // validate current midi note and switch to next note:
		toggleGreenLED();
		printf("dist(%lu)=%lu and dist(%lu)=%lu\n",
				vco->cv,
				vco->current_interval - note_To_Timer[vco->note],
				vco->cv-1,
				vco->previous_interval - note_To_Timer[vco->note]);

		// would previous DAC level match better?
		if (abs(vco->current_interval - note_To_Timer[vco->note]) > abs(vco->previous_interval - note_To_Timer[vco->note])){
			vco->note_to_cv[vco->note] = vco->cv-1;
		}
		else
			vco->note_to_cv[vco->note] = vco->cv;
		printf("note=%lu -> dac=%lu\n", vco->note, vco->note_to_cv[vco->note]);
		vco->note++;
	}

	// finish and switch to next VCO or terminate calib process:
	if (vco->note > 127 || vco->cv > vco->cv_max){

		HAL_TIM_IC_Stop_IT(htimVcoCalib, vco->IC_Channel);
		dacWrite_Blocking(0, vco->dac);
		vco->completed = TRUE;

	}
}



