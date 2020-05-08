/*
 * vco_calibration.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_VCO_CALIBRATION_H_
#define INC_VCO_CALIBRATION_H_


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
#define TIM_CHANNEL_CALIB_VCO3340A TIM_CHANNEL_1 // PA8 (TIM1)
#define TIM_CHANNEL_CALIB_VCO3340B TIM_CHANNEL_2 // PA9 (TIM1)
#define TIM_CHANNEL_CALIB_VCO13700 TIM_CHANNEL_3 // PA10 (TIM1)

#define CCR_VCO3340A CCR1
#define CCR_VCO3340B CCR2
#define CCR_VCO13700 CCR3

// see MX_TIM2_Init in tim.c:
/*
#define TIMCALIB_PRESCALER 16.0 // htim2.Init.Prescaler+1
#define TIMCALIB_PERIOD (TIMCALIB_PRESCALER / 108.0e6)
#define TIMCALIB_FREQ (1.0/TIMCALIB_PERIOD)
#define TIMCALIB_IC_PRESCALER 1
*/

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


/* function prototypes -----------------------------------------------*/

void runVcoCalibration();
void VCO_Calib_CaptureCallback();

//#define CALIBRATION_PERIODS 4



#endif /* INC_VCO_CALIBRATION_H_ */
