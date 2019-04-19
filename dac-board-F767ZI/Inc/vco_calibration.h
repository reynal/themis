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
	CALIB_VCO_3340,  // 1, CEM3340 VCO
	CALIB_VCO_13700  // 2, LM13700 VCO
} vcoCalib_t;

// aliases for easier code reading
#define TIM_CHANNEL_CALIB_VCO3340 TIM_CHANNEL_1
#define TIM_CHANNEL_CALIB_VCO13700 TIM_CHANNEL_4

// see MX_TIM2_Init in main.c:
#define TIM2_PRESCALER 17.0
#define TIM2_PERIOD (TIM2_PRESCALER / 54.0e6)
#define TIM2_FREQ (1.0/TIM2_PERIOD)
#define TIM2_IC_PRESCALER 1

// MIDI and VCO related:
#define BASE_MIDI_FREQ 27.5 // LA  1

#define VCO3340_MIN_INPUT_CV 100
#define VCO3340_MAX_INPUT_CV 2200 // 2400 // max admissible input in 0-4096mV range (beyond which oscillator ceases to work)

#define VCO13700_MIN_INPUT_CV 1000
#define VCO13700_MAX_INPUT_CV 3000 // max admissible input in 0-4096mV range (beyond which oscillator ceases to work)

/*
 *
 *  fcapture = TIM2_FREQ / TIM2_PRESCALER * TIM2_IC_PRESCALER / capture_diff
 *
 * capture_diff (expected for a given note ) = TIM2_FREQ / TIM2_PRESCALER * TIM2_IC_PRESCALER / fnote
 *
 */

void calibrateVcoCallbackShort();
void calibrateVcoCallbackLong();
void launchVcoCalibration();
void initMidiToFreqArray();
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void fillMidiToCVArray(int* midiToVcoCV);

#define CALIBRATION_PERIODS 4

#endif /* VCO_CALIBRATION_H_ */
