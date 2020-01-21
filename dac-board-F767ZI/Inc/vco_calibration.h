/*
 * vco_calibration.h
 *
 *  Created on: Jan 31, 2019
 *      Author: Morgan / Reynal
 */

#ifndef __VCO_calibration_H
#define __VCO_calibration_H

/*
 * an enum of constants to specify e.g. which VCO we currently calibrate
 */
typedef enum {
	CALIB_COMPLETED, // 0
	CALIB_VCO_3340A,  // 1, CEM3340 VCO
	CALIB_VCO_3340B,  // 2, CEM3340 VCO
	CALIB_VCO_13700  // 3, LM13700 VCO
} vcoCalib_t;

// aliases for easier code reading
#define TIM_CHANNEL_CALIB_VCO3340A TIM_CHANNEL_1 // PA0
#define TIM_CHANNEL_CALIB_VCO3340B TIM_CHANNEL_3 // PB10
#define TIM_CHANNEL_CALIB_VCO13700 TIM_CHANNEL_4 // PA3

// see MX_TIM2_Init in main.c:
#define TIM2_PRESCALER 16.0 // htim2.Init.Prescaler+1
#define TIM2_PERIOD (TIM2_PRESCALER / 108.0e6)
#define TIM2_FREQ (1.0/TIM2_PERIOD)
#define TIM2_IC_PRESCALER 1

// MIDI and VCO related:
#define A4_FREQ 440.0 // LA  4
#define A4_PERIOD 1.0/A4_FREQ
#define A4_MIDI_NOTE 69

#define VCO3340A_MIN_INPUT_CV 0
#define VCO3340A_MAX_INPUT_CV 4095

#define VCO3340B_MIN_INPUT_CV 0
#define VCO3340B_MAX_INPUT_CV 4095

#define VCO13700_MIN_INPUT_CV 0
#define VCO13700_MAX_INPUT_CV 4095

/*
 *
 *  fcapture = TIM2_FREQ / TIM2_PRESCALER * TIM2_IC_PRESCALER / capture_diff
 *
 * capture_diff (expected for a given note ) = TIM2_FREQ / TIM2_PRESCALER * TIM2_IC_PRESCALER / fnote
 *
 */

/* Private function prototypes -----------------------------------------------*/

void startCalib3340A();
void startCalib3340B();
void startCalib13700();
void runVcoCalibration();
int  midiNoteToTimerInterval(int);
void printMidiToVCOCVTables();
void resetMidiToVCOCVTables();
void VCO_Calib_CaptureCallback();

//#define CALIBRATION_PERIODS 4

#endif /* VCO_CALIBRATION_H_ */
