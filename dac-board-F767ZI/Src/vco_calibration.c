/*
 * vco_calibration.c
 *
 *  Created on: 20 nov. 2018
 *      Author: Morgan PRIOTON / Reynal
 *
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
 * VCO 3340 is able to produce a 17Hz a the lowest note though it can hardly heard (CV at 0mV).
 * The highest note is close to 1kHz (TODO: check exactly) (= 1562mV on the CV input, i.e, 3200 on the dac with gain=2)
 *
 * VCO13700 ... ??? TODO @Nathan
 *
 *
 *Optimal freq range is
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "main.h"
#include "vco_calibration.h"
#include "dac_board.h"
#include "math.h"
#include "stdio.h"

/* Private variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef* htimCalib;
extern TIM_HandleTypeDef* htimEnveloppes;
extern int midiToVCO3340CV[128];
extern int midiToVCO13700CV[128];

int calibrationVcoDAC; // VCO Control Voltage, i.e., DAC output !
int calibrationLastCapture = -1; // stores the Capture register value ; -1 when uninitialized
int calibrationTimerInterval;

vcoCalib_t calibrationCurrentVco; // status of the calibration operation

int maxVcoDAC = 4096; // allowed DAC range for each VCO
int minVcoDAC = 0;
int* midiToVCOCV; // points to midiToVCO3340CV or midiToVCO13700CV

int currentMidiNote = -1; // -1 when uninitialized

int captureTimerConversionFactor; // convert from a period value (in s) to a counting interval for the htimCalib timer, used by midiNoteToTimerInterval()


/**
 * Launch the calibration process
 * 
 * First, we start the calibration process with VCO 3340.
 * So, we set the configuration of the device and we launch the calibration process.
 */
void runVcoCalibration(){

	printf("Starting calibration...\n");

	HAL_TIM_Base_Stop_IT(htimEnveloppes); // stop timer responsible for updating ADSR enveloppes

	captureTimerConversionFactor = (int)(8.0 * 54.0e6 / (htimCalib->Init.Prescaler+1.0));

	// start with VCO3340:
	startCalib3340();

	while(calibrationCurrentVco != CALIB_COMPLETED){
		  HAL_Delay(100); // 100ms => LED blinks faster so we know we're calibrating!
		  //HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
	}

	// proceeds with main() code
	printf("Calibration terminated!\n");
	HAL_TIM_Base_Start_IT(htimEnveloppes); // restart timer responsible for updating ADSR enveloppes


}

void startCalib3340(){

	calibrationCurrentVco = CALIB_VCO_3340;
	midiToVCOCV = midiToVCO3340CV;
	maxVcoDAC = VCO3340_MAX_INPUT_CV;
	calibrationVcoDAC = minVcoDAC = VCO3340_MIN_INPUT_CV; // don't start too low... stability issues !
	dacWrite(calibrationVcoDAC, DAC_VCO_3340_FREQ);
	__HAL_TIM_SetCounter(htimCalib, 0); // avoid overshoots
	HAL_TIM_IC_Start_IT(htimCalib, TIM_CHANNEL_CALIB_VCO3340);
}

void startCalib13700(){

	calibrationCurrentVco = CALIB_VCO_13700;
	midiToVCOCV = midiToVCO13700CV;
	maxVcoDAC = VCO13700_MAX_INPUT_CV;
	calibrationVcoDAC = minVcoDAC = VCO13700_MIN_INPUT_CV; // don't start too low... stability issues !
	dacWrite(calibrationVcoDAC, DAC_VCO_13700);
	__HAL_TIM_SetCounter(htimCalib, 0); // avoid overshoots
	HAL_TIM_IC_Start_IT(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
}

/**
 * Returns the counting interval of the capture timer that yields a signal period
 * matching the given midiNote parameter. Computation are based on the equal tempered range.
 *
 * For example, midiNote = 69 => f = 440Hz => T = 1/440 => counting_interval = T * captureTimerConversionFactor
 */
int midiNoteToTimerInterval(int midiNote){

	return (int)(captureTimerConversionFactor * A4_PERIOD * pow(2.0, (A4_MIDI_NOTE - midiNote)/12.0));

}

/**
 * interrupt callback that gets triggered whenever a rising edge is detected on the current VCO calibration pin
 * the global var calibrationTimerInterval is updated after each call
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	if (htim != htimCalib) return;

	// compute timer interval b/w subsequent edges for input signal and store the result in global var calibrationTimerInterval:
	int x=0;

	switch (calibrationCurrentVco) {
	case CALIB_VCO_3340: // ch 1
		x = htimCalib->Instance->CCR1;
		break;
	case CALIB_VCO_13700: // ch 2
		x = htimCalib->Instance->CCR4;
		break;
	case CALIB_COMPLETED:
		return;
	}


	if (calibrationLastCapture == -1) { // init calibrationLastCapture variable
		calibrationLastCapture = x;
		return;
	}

	calibrationTimerInterval = x - calibrationLastCapture;
	calibrationLastCapture = x;

	// compute initial (lowest) currentMidiNote: we assume that the DAC level is at its minimum, so that the VCO outputs a signal with
	// the largest possible period and then we'll look for the closest midi note which we'll be the one we start the calibration process with:
	if (currentMidiNote == -1){

		currentMidiNote = 0;
		// iterate over currentMidiNote until corresponding timer interval is lower or equal to the period of the current signal:
		while (currentMidiNote < 127 && midiNoteToTimerInterval(currentMidiNote) > calibrationTimerInterval){
			currentMidiNote++;
		}

		//for (int i=0; i<currentMidiNote; i++) midiToVCOCV[i] = 0;

	}

	// look for next DAC level for which the VCO signal has the closest possible period to midiNoteToTimerInterval(currentMidiNote):
	if (midiNoteToTimerInterval(currentMidiNote) < calibrationTimerInterval) {
		calibrationVcoDAC ++; // try to decrease signal period until there's a match
		if (calibrationVcoDAC <= maxVcoDAC) dacWrite(calibrationVcoDAC, DAC_VCO_3340_FREQ); // else see below
	}
	else { // validate current midi note and switch to next note:
		HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
		printf("Calib: %d -> %d\n", currentMidiNote, calibrationVcoDAC);
		midiToVCOCV[currentMidiNote] = calibrationVcoDAC;
		//printf("array: %d -> %d\n", currentMidiNote, midiToVCOCV[currentMidiNote]);
		currentMidiNote++;
	}

	if (currentMidiNote > 127 || calibrationVcoDAC > maxVcoDAC){ // finish and switch to next VCO or terminate calib process

		switch (calibrationCurrentVco) {
			case CALIB_VCO_3340:
				HAL_TIM_IC_Stop_IT(htimCalib, TIM_CHANNEL_CALIB_VCO3340);
				calibrationCurrentVco=CALIB_COMPLETED; // debug
				// debug startCalib13700();
				break;
			case CALIB_VCO_13700:
				HAL_TIM_IC_Stop_IT(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
				calibrationCurrentVco=CALIB_COMPLETED; // ok over
				break;
			case CALIB_COMPLETED:
				return;
		}
	}
}

