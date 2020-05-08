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
 * VCO 3340 is able to produce a 35Hz as the lowest note (CV at 0mV).
 *
 *
 *
 *
 * Optimal freq range is ?
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32l4xx_hal.h"
#include "tim.h"
#include "main.h"
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
extern int midiToVCO3340ACV[128];
extern int midiToVCO3340BCV[128];
extern int midiToVCO13700CV[128];

/* variables ---------------------------------------------------------*/


int calibrationVcoDACLvl; // VCO Control Voltage, i.e., DAC output !
int calibrationLastCapture = -1; // stores the Capture register value ; -1 when uninitialized
int previousCalibrationTimerInterval;
int calibrationTimerInterval;

vcoCalib_t calibrationCurrentVco; // status of the calibration operation

int maxVcoDAC = 4096; // allowed DAC range for each VCO
int minVcoDAC = 0;

int* midiToVCOCV; // points to midiToVCO3340CV or midiToVCO13700CV

int currentMidiNote = -1; // -1 when uninitialized

int time_To_Timer_Interval_Conversion_Factor; // convert from a period value (in s) to a counting interval for the htimCalib timer, used by midiNoteToTimerInterval()

int dbgCounter=0;
int dbgValues[10] = {0};
int dbgDeltas[10] = {0};

/* function prototypes -----------------------------------------------*/

static int getTimerIntervalFromMidiNote(int midiNote);
static void startCalib3340A();
static void startCalib3340B();
static void startCalib13700();
static void printMidiToVCOCVTables();
static void resetMidiToVCOCVTables();
static void init_Time_To_Timer_Interval_Conversion_Factor();

/* user code -----------------------------------------------*/


/*
 * Compute timer_To_Midi_Conversion_Factor
 */
static void init_Time_To_Timer_Interval_Conversion_Factor(){

	  /* Get PCLK2 frequency for TIM1 */
	  double pclk = HAL_RCC_GetPCLK2Freq(); // 80MHz

	  /* Get PCLK prescaler */
	  if((RCC->CFGR & RCC_CFGR_PPRE2) != 0) pclk *= 2; // currently we have prescaler equal to 1

	  int prescaler = 8; // see tim.c : sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	  pclk /= prescaler;

	  time_To_Timer_Interval_Conversion_Factor = (int)(pclk / (htimVcoCalib->Instance->PSC + 1));

}

/**
 * Launch the calibration process
 *
 * First, we start the calibration process with VCO 3340.
 * So, we set the configuration of the device and we launch the calibration process.
 */
void runVcoCalibration(){

	dac_Board_Timer_Stop(); // stop timer responsible for updating ADSR enveloppes
	HAL_Delay(10); // wait until current DMA transfer are finished

	prepareVCOForCalibration();

	resetMidiToVCOCVTables(); // optional, could be commented out if we want to refine only a part of the calibration table


	init_Time_To_Timer_Interval_Conversion_Factor();

	// first start with VCO3340:
	startCalib3340A();
	//startCalib3340B();
	//startCalib13700();

	while(calibrationCurrentVco != CALIB_COMPLETED){
		  HAL_Delay(50); // LED blinks faster so we know we're calibrating!
		  toggleRedLED();
	}

	// proceeds with main() code
	//printf("Calibration terminated!\n");
	printMidiToVCOCVTables();

	// debug L4 dac_Board_Timer_Start(); // restart timer responsible for updating ADSR enveloppes


}


static void startCalib3340A(){

	printf("Starting 3340 A calibration...\n");

	currentMidiNote = -1;
	calibrationLastCapture = -1;
	calibrationTimerInterval = 0;
	calibrationCurrentVco = CALIB_VCO_3340A;
	midiToVCOCV = midiToVCO3340ACV;
	maxVcoDAC = VCO3340A_MAX_INPUT_CV;
	calibrationVcoDACLvl = minVcoDAC = VCO3340A_MIN_INPUT_CV;
	dacWrite_Blocking(calibrationVcoDACLvl, DAC_VCO_3340A_FREQ);
	__HAL_TIM_SetCounter(htimVcoCalib, 0); // avoid overshoots
	HAL_TIM_IC_Start_IT(htimVcoCalib, TIM_CHANNEL_CALIB_VCO3340A);
}

static void startCalib3340B(){

	printf("Starting 3340 B calibration...\n");

	currentMidiNote = -1;
	calibrationLastCapture = -1;
	calibrationTimerInterval = 0;
	calibrationCurrentVco = CALIB_VCO_3340B;
	midiToVCOCV = midiToVCO3340BCV;
	maxVcoDAC = VCO3340B_MAX_INPUT_CV;
	calibrationVcoDACLvl = minVcoDAC = VCO3340B_MIN_INPUT_CV;
	dacWrite_Blocking(calibrationVcoDACLvl, DAC_VCO_3340B_FREQ);
	__HAL_TIM_SetCounter(htimVcoCalib, 0); // avoid overshoots
	HAL_TIM_IC_Start_IT(htimVcoCalib, TIM_CHANNEL_CALIB_VCO3340B);
}

static void startCalib13700(){

	printf("Starting 13700 calibration...\n");

	currentMidiNote = -1;
	calibrationLastCapture = -1;
	calibrationTimerInterval = 0;
	calibrationCurrentVco = CALIB_VCO_13700;
	midiToVCOCV = midiToVCO13700CV;
	maxVcoDAC = VCO13700_MAX_INPUT_CV;
	calibrationVcoDACLvl = minVcoDAC = VCO13700_MIN_INPUT_CV; // don't start too low... stability issues !
	dacWrite_Blocking(calibrationVcoDACLvl, DAC_VCO_13700_FREQ);
	__HAL_TIM_SetCounter(htimVcoCalib, 0); // avoid overshoots
	HAL_TIM_IC_Start_IT(htimVcoCalib, TIM_CHANNEL_CALIB_VCO13700);
}

/**
 * Returns the counting interval of the capture timer that yields a signal period
 * matching the given midiNote parameter. Computation are based on the equal tempered scale.
 *
 * For example, midiNote = 69 => f = 440Hz => T = 1/440 => counting_interval = T * captureTimerConversionFactor
 */
static int getTimerIntervalFromMidiNote(int midiNote){

	return (int)(time_To_Timer_Interval_Conversion_Factor * A4_PERIOD * pow(2.0, (A4_MIDI_NOTE - midiNote)/12.0));

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
static void printMidiToVCOCVTables(){

	printf("\n---- calib data ----\n\n");
	printf("int midiToVCO3340ACV[128] = {\n");
	for (int midiNote = 0; midiNote < 126 ; midiNote++){
		printf("%d, ", midiToVCO3340ACV[midiNote]);
		if (midiNote % 12 ==11 ) printf("\n");
	}
	printf("%d };\n\n", midiToVCO3340ACV[126]);

	printf("int midiToVCO3340BCV[128] = {\n");
	for (int midiNote = 0; midiNote < 126 ; midiNote++){
		printf("%d, ", midiToVCO3340BCV[midiNote]);
		if (midiNote % 12 ==11 ) printf("\n");
	}
	printf("%d };\n\n", midiToVCO3340BCV[126]);

	printf("int midiToVCO13700CV[128] = {\n");
	for (int midiNote = 0; midiNote < 126 ; midiNote++){
		printf("%d, ", midiToVCO13700CV[midiNote]);
		if (midiNote % 12 ==11 ) printf("\n");
	}
	printf("%d };\n", midiToVCO13700CV[126]);
}

/**
 * reset content of Midi Note to CV arrays
 */
static void resetMidiToVCOCVTables(){

	for (int midiNote = 0; midiNote < 127 ; midiNote++){
		midiToVCO3340ACV[midiNote] = 0;
		midiToVCO3340BCV[midiNote] = 0;
		midiToVCO13700CV[midiNote] = 0;
	}


}

void VCO_Calib_CaptureCallback(){

	int x = htimVcoCalib->Instance->CCR_VCO3340A;
	dbgValues[dbgCounter] = x;
	if (dbgCounter>0) dbgDeltas[dbgCounter] = x - dbgValues[dbgCounter-1];
	if (dbgDeltas[dbgCounter]<0) {
		dbgDeltas[dbgCounter] += 65536;
		//dbgDeltas[dbgCounter] = -dbgDeltas[dbgCounter];
	}
	dbgCounter++;
	if (dbgCounter == 10) {
		dbgCounter=0;
		//__HAL_TIM_SetCounter(htimVcoCalib, 0);
	}


}

/**
 * interrupt callback that gets triggered whenever a rising edge is detected on the current VCO calibration pin
 * the global var calibrationTimerInterval is updated after each call
 */
void _VCO_Calib_CaptureCallback(){

	// compute timer interval b/w subsequent edges for input signal and store the result in global var "calibrationTimerInterval":
	int x=0;

	if (calibrationCurrentVco == CALIB_VCO_3340A) x = htimVcoCalib->Instance->CCR_VCO3340A;
	else if (calibrationCurrentVco == CALIB_VCO_3340B) x = htimVcoCalib->Instance->CCR_VCO3340B;
	else if (calibrationCurrentVco == CALIB_VCO_13700) x = htimVcoCalib->Instance->CCR_VCO13700;
	else return; // CALIB_COMPLETED !!!

	if (calibrationLastCapture == -1) { // init calibrationLastCapture variable
		calibrationLastCapture = x;
		return;
	}

	previousCalibrationTimerInterval = calibrationTimerInterval;
	calibrationTimerInterval = x - calibrationLastCapture;
	calibrationLastCapture = x;

	// compute initial (lowest) currentMidiNote: we assume that the DAC level is at its minimum, so that the VCO outputs a signal with
	// the largest possible period and then we'll look for the closest midi note which we'll be the one we start the calibration process with:
	if (currentMidiNote == -1){

		currentMidiNote = 0;
		// increment currentMidiNote from 0 until the corresponding timer interval is lower or equal to the period of the current VCO signal:
		while (currentMidiNote < 127 && getTimerIntervalFromMidiNote(currentMidiNote) > calibrationTimerInterval){
			currentMidiNote++;
		}

	}

	// look for next DAC level for which the VCO signal has the closest possible period to getTimerIntervalFromMidiNote(currentMidiNote):
	if (calibrationTimerInterval > getTimerIntervalFromMidiNote(currentMidiNote)) {
		calibrationVcoDACLvl ++; // try to decrease signal period until there's a match
		if (calibrationVcoDACLvl <= maxVcoDAC) { // else see below
			// TODO L4 if (calibrationCurrentVco == CALIB_VCO_3340A) dacWrite(calibrationVcoDACLvl, DAC_VCO_3340A_FREQ);
			// TODO L4 else if (calibrationCurrentVco == CALIB_VCO_3340B) dacWrite(calibrationVcoDACLvl, DAC_VCO_3340B_FREQ);
			// TODO L4 else if (calibrationCurrentVco == CALIB_VCO_13700) dacWrite(calibrationVcoDACLvl, DAC_VCO_13700_FREQ);
			// TODO L4 else return; // calib completed
		}
	}
	else { // validate current midi note and switch to next note:
		toggleGreenLED();
		printf("d(%d)=%d and d(%d)=%d\n", calibrationVcoDACLvl, calibrationTimerInterval-getTimerIntervalFromMidiNote(currentMidiNote),
				calibrationVcoDACLvl-1, previousCalibrationTimerInterval-getTimerIntervalFromMidiNote(currentMidiNote));

		// check if previous DAC level doesn't match better:
		if (abs(calibrationTimerInterval-getTimerIntervalFromMidiNote(currentMidiNote)) > abs(previousCalibrationTimerInterval-getTimerIntervalFromMidiNote(currentMidiNote))){
			midiToVCOCV[currentMidiNote] = calibrationVcoDACLvl-1;
		}
		else midiToVCOCV[currentMidiNote] = calibrationVcoDACLvl;
		printf("note=%d -> dac=%d\n", currentMidiNote, midiToVCOCV[currentMidiNote]);
		currentMidiNote++;
	}

	// finish and switch to next VCO or terminate calib process:
	if (currentMidiNote > 127 || calibrationVcoDACLvl > maxVcoDAC){

		switch (calibrationCurrentVco) {
			case CALIB_VCO_3340A:
				HAL_TIM_IC_Stop_IT(htimVcoCalib, TIM_CHANNEL_CALIB_VCO3340A);
				// TODO L4 dacWrite(0, DAC_VCO_3340A_FREQ);
				//calibrationCurrentVco=CALIB_COMPLETED; // debug
				//startCalib13700();
				startCalib3340B();
				break;
			case CALIB_VCO_3340B:
				HAL_TIM_IC_Stop_IT(htimVcoCalib, TIM_CHANNEL_CALIB_VCO3340B);
				// TODO L4 dacWrite(0, DAC_VCO_3340B_FREQ);
				calibrationCurrentVco=CALIB_COMPLETED; // debug
				//startCalib3340B();
				break;
			case CALIB_VCO_13700:
				HAL_TIM_IC_Stop_IT(htimVcoCalib, TIM_CHANNEL_CALIB_VCO13700);
				calibrationCurrentVco=CALIB_COMPLETED; // ok over
				// TODO L4 dacWrite(0, DAC_VCO_13700_FREQ);
				break;
			case CALIB_COMPLETED:
				return;
		}
	}
}



