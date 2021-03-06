/*
 * vco_calibration.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_VCO_CALIBRATION_H_
#define INC_VCO_CALIBRATION_H_

#define VCO_COUNT 3

#include "ad5644.h"

typedef struct {
	char name[10]; // e.g. "VCO3340A"
	Dac dac; // DAC channel linked to this VCO
	int note; // init to -1
	uint32_t cv; // Control voltage (aka DAC output)
	uint32_t cv_min; // lowest possible CV
	uint32_t cv_max; // highest possible CV
	int previous_capture; // init to -1; stores the Capture register value for interval calculation ; -1 when uninitialized
	int previous_interval; // timer interval, that is, current capture reg - previous capture reg
	int current_interval;
	uint32_t * note_to_cv; // calibration table
	uint32_t IC_Channel;
	Boolean completed;
} Vco_Calib;

// aliases for easier code reading
#define TIM_CHANNEL_CALIB_VCO3340A TIM_CHANNEL_1 // PA8 (TIM1)
#define TIM_CHANNEL_CALIB_VCO3340B TIM_CHANNEL_2 // PA9 (TIM1)
#define TIM_CHANNEL_CALIB_VCO13700 TIM_CHANNEL_3 // PA10 (TIM1)

#define CCR_VCO3340A CCR1
#define CCR_VCO3340B CCR2
#define CCR_VCO13700 CCR3

#define A4_FREQ 440.0 // LA  4
#define A4_PERIOD 1.0/A4_FREQ
#define A4_MIDI_NOTE 69

#define VCO3340A_MIN_INPUT_CV 0
#define VCO3340A_MAX_INPUT_CV 4095

#define VCO3340B_MIN_INPUT_CV 2400
#define VCO3340B_MAX_INPUT_CV 4095

#define VCO13700_MIN_INPUT_CV 0
#define VCO13700_MAX_INPUT_CV 4095


/* function prototypes -----------------------------------------------*/

void vcoCalib_Run();
void vcoCalib_IC_IRQHandler(uint32_t IC_Channel);
void vcoCalib_UP_IRQHandler();

#endif /* INC_VCO_CALIBRATION_H_ */
