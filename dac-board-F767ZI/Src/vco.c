/*
 * vco.c
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey

 * ----------------- mixers ----------------
 * V2164D : -33mV/dB => with a 2V input range, this yields 60dB of attenuation.
 *
 * There are two mixers with four inputs each. Inputs are numbered from 1 to 8 :
 *
 * in1 : VCO 3340 #B TRI
 * in2 : VCO 3340 #B SAW
 * in3 : VCO 3340 #B PULSE
 * in4 : -
 *
 * in5 : -
 * in6 : -
 * in7 : VCO 3340 #A
 * in8 : VCA (separate channel that is fed by the output of the VCF)

 * 3340A : the DG411 makes it possible to chose between TRI, SAW and SQU output ; the last switch of the IC can enable the VCO synchronisation 13700 -> 3340.

 */

/* Includes ------------------------------------------------------------------*/

#include "vco.h"
#include "stm32f7xx_hal.h"
#include "main.h"
#include "mcp4822.h"
#include "midi.h"
#include "dac_board.h"
#include "stdio.h"

/* External variables --------------------------------------------------------*/

extern MidiNote midiNote;

/* Private variables ---------------------------------------------------------*/

Vco13700Parameters vco13700 = {
		.detune=NO_DETUNE,
		.semitones=NO_SHIFT,
		.octave=CENTER_OCTAVE,
		.tri_level=DEF_MIDICC_VCO13700_TRI_LEVEL,
		.squ_level=DEF_MIDICC_VCO13700_SQU_LEVEL,
		.subbass_level=DEF_MIDICC_VCO13700_SUBBASS_LEVEL
};

Vco3340AParameters vco3340A = {
		.detune=NO_DETUNE,
		.semitones=NO_SHIFT,
		.octave=CENTER_OCTAVE,
		.level=DEF_MIDICC_VCO3340A_LEVEL,
		.pwm=DEF_MIDICC_VCO3340A_PWM_DUTY
};

Vco3340BParameters vco3340B = {
		.detune=NO_DETUNE,
		.semitones=NO_SHIFT,
		.octave=CENTER_OCTAVE,
		.pwm=DEF_MIDICC_VCO3340B_PWM_DUTY,
		.tri_level = DEF_MIDICC_VCO3340B_TRI_LEVEL,
		.saw_level = DEF_MIDICC_VCO3340B_SAW_LEVEL,
		.pulse_level = DEF_MIDICC_VCO3340B_PULSE_LEVEL
};


// an array that maps a MIDI note to a DAC value for the VCO DAC (from 0 to 4095): e.g. note 36 maps to 442
// this array is normally filled by the calibration process!
// (diff between notes is b/w 54 and 59)

int midiToVCO3340ACV[128] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 60, 114, 171, 227, 283, 339, 394, 451,
504, 562, 617, 674, 731, 787, 845, 900, 957, 1011, 1069, 1125,
1181, 1238, 1294, 1353, 1408, 1466, 1521, 1578, 1634, 1691, 1747, 1804,
1860, 1917, 1974, 2030, 2086, 2143, 2199, 2256, 2312, 2369, 2426, 2483,
2539, 2594, 2650, 2707, 2763, 2821, 2877, 2934, 2990, 3047, 3103, 3159,
3215, 3271, 3329, 3385, 3442, 3498, 3554, 3609, 3665, 3722, 3777, 3836,
3891, 3949, 4005, 4061, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0 };

int midiToVCO3340BCV[128] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 85, 142, 199, 256, 312, 370, 426, 483,
539, 595, 653, 709, 766, 822, 879, 936, 993, 1050, 1106, 1164,
1220, 1279, 1335, 1392, 1449, 1505, 1561, 1618, 1675, 1732, 1789, 1846,
1902, 1959, 2015, 2071, 2128, 2184, 2243, 2298, 2356, 2412, 2469, 2525,
2581, 2639, 2694, 2753, 2809, 2868, 2925, 2983, 3039, 3095, 3153, 3209,
3268, 3323, 3382, 3437, 3495, 3551, 3608, 3665, 3721, 3780, 3835, 3894,
3949, 4007, 4064, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0 };


int midiToVCO13700CV[128] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 21, 77, 132, 192, 247, 305, 361, 361, 474,
530, 587, 643, 701, 757, 814, 869, 925, 982, 1037, 1095, 1150,
1208, 1263, 1320, 1376, 1433, 1490, 1545, 1602, 1657, 1715, 1770, 1827,
1884, 1940, 1998, 2053, 2112, 2167, 2224, 2281, 2338, 2395, 2451, 2509,
2562, 2621, 2677, 2735, 2790, 2847, 2904, 2960, 3017, 3071, 3129, 3186,
3243, 3299, 3355, 3412, 3468, 3526, 3580, 3637, 3693, 3750, 3807, 3863,
3920, 3976, 4034, 4089, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0 };


/* Private function prototypes -----------------------------------------------*/

void initVco(){

	setVco3340AWaveType(DEF_MIDICC_VCO3340A_WAVE);
	setVco3340ASync(0);

	midiNote.note = DEF_MIDI_NOTE;

	setVco3340APWMDuty(DEF_MIDICC_VCO3340A_PWM_DUTY);
	setVco3340BPWMDuty(DEF_MIDICC_VCO3340B_PWM_DUTY);

	// mute unused mixers:
	dacWrite(4095, DAC_V2140D_IN4); HAL_Delay(1); // not connected yet
	dacWrite(4095,DAC_V2140D_IN5); HAL_Delay(1);  // not connected yet
	dacWrite(4095,DAC_V2140D_IN6); HAL_Delay(1);  // not connected yet

	setVco3340ALevel(DEF_MIDICC_VCO3340A_LEVEL);
	setVco3340BPulseLevel(DEF_MIDICC_VCO3340B_PULSE_LEVEL);
	setVco3340BSawLevel(DEF_MIDICC_VCO3340B_SAW_LEVEL);
	setVco3340BTriLevel(DEF_MIDICC_VCO3340B_TRI_LEVEL);
	setVco13700SquareLevel(DEF_MIDICC_VCO13700_SQU_LEVEL);
	setVco13700TriLevel(DEF_MIDICC_VCO13700_TRI_LEVEL);
	setVco13700SubbassLevel(DEF_MIDICC_VCO13700_SUBBASS_LEVEL);


}

// used during calibration:
void prepareVCOForCalibration(){

	HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_RESET); // "unplug" 3340A vco output from mixer so as
	HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_RESET); // to reduce interference with other VCO
	HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_RESET);
	dacWrite(MAX_VCO3340A_PWM_DUTY, DAC_VCO_3340A_PWM_DUTY); HAL_Delay(1); // make sure VCO calibration output
	dacWrite(MAX_VCO3340B_PWM_DUTY, DAC_VCO_3340B_PWM_DUTY); HAL_Delay(1); // is a square (i.e., 50% duty cycle)

}

// ---------------------------------------------------------------------------------------------------
//                      vco 13700
// ---------------------------------------------------------------------------------------------------

void setVco13700Octave(uint8_t midiValue){
	if (midiValue > 4 || midiValue < 0) midiValue = CENTER_OCTAVE;
	vco13700.octave = midiValue;
}

void setVco13700Semitones(uint8_t midiValue){
	if (midiValue > 22 || midiValue < 0) midiValue = NO_SHIFT;
	vco13700.semitones = midiValue;
}

void setVco13700Detune(uint8_t value){
	vco13700.detune = value;
}

void updateVco13700Freq(){

	int dacLvl = midiToVCO13700CV[midiNote.note + 12 * (vco13700.octave-CENTER_OCTAVE) + (vco13700.semitones-NO_SHIFT)] + vco13700.detune - NO_DETUNE;
	dacWrite(dacLvl, DAC_VCO_13700_FREQ);

}

void setVco13700SquareLevel(uint8_t midiValue){
	vco13700.squ_level = midiValue;
}

void updateVco13700SquareLevel(){
	dacWrite((int)(MAX_VCO13700_SQU_LEVEL * (127.0-vco13700.squ_level) /127.0), DAC_V2140D_13700_SQU_LVL);
}

void setVco13700TriLevel(uint8_t midiValue){
	vco13700.tri_level = midiValue;
}

void updateVco13700TriLevel(){
	dacWrite((int)(MAX_VCO13700_TRI_LEVEL * (127.0-vco13700.tri_level) /127.0), DAC_V2140D_13700_TRI_LVL);
}

void setVco13700SubbassLevel(uint8_t midiValue){
	vco13700.subbass_level = midiValue;
}

void updateVco13700SubbassLevel(){
	dacWrite((int)(MAX_VCO13700_SUBBASS_LEVEL * (127.0-vco13700.subbass_level) /127.0), DAC_V2140D_13700_SUBBASS_LVL);
}


// ---------------------------------------------------------------------------------------------------
//                       vco 3340A (the one with the analog switch IC plugged in)
// ---------------------------------------------------------------------------------------------------

// a midi value of 2 yields the center octave ; a value of 0 leads to -2 octaves ; a value of 4 leads to +2 octaves
void setVco3340AOctave(uint8_t midiValue){
	if (midiValue > 4 || midiValue < 0) midiValue = CENTER_OCTAVE;
	vco3340A.octave = midiValue;
}

void setVco3340ASemitones(uint8_t midiValue){
	if (midiValue > 22 || midiValue < 0) midiValue = NO_SHIFT;
	vco3340A.semitones = midiValue;
}


void setVco3340ADetune(uint8_t value){
	vco3340A.detune = value;
}

void updateVco3340AFreq(){
	int dacLvl = midiToVCO3340ACV[midiNote.note + 12 * (vco3340A.octave-CENTER_OCTAVE) + (vco3340A.semitones-NO_SHIFT)] + vco3340A.detune - NO_DETUNE;
	dacWrite(dacLvl, DAC_VCO_3340A_FREQ);

}

void setVco3340APWMDuty(uint8_t midiValue){
	vco3340A.pwm = midiValue;
}

void updateVco3340APWMDuty(){
	dacWrite((int) (MAX_VCO3340A_PWM_DUTY * vco3340A.pwm/127.), DAC_VCO_3340A_PWM_DUTY);
}

void setVco3340AWaveType(uint8_t midiValue){
	midiValue = midiValue % 3;
	if (midiValue == 2){ // pulse
		HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_SET);
	}
	else if (midiValue == 1){ // sawtooth
		HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_SET);
	}
	else if (midiValue == 0){ // triangle
		HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_SET);
	}
}

void setVco3340ASync(uint8_t midiValue){
	HAL_GPIO_WritePin(SYNC_3340_GPIO_Port, SYNC_3340_Pin, midiValue>=64 ? GPIO_PIN_SET:GPIO_PIN_RESET);
}

void setVco3340ALevel(uint8_t midiValue){
	vco3340A.level = midiValue;
}

void updateVco3340ALevel(){
	dacWrite((int) (MAX_VCO3340A_LEVEL * (127.0-vco3340A.level)/127.), DAC_V2140D_3340A_LVL);
}

// ---------------------------------------------------------------------------------------------------
//               vco 3340B (the one without the analog switch IC plugged in)
// ---------------------------------------------------------------------------------------------------

void setVco3340BOctave(uint8_t midiValue){
	if (midiValue > 4 || midiValue < 0) midiValue = CENTER_OCTAVE;
	vco3340B.octave = midiValue;
}

void setVco3340BSemitones(uint8_t midiValue){
	if (midiValue > 22 || midiValue < 0) midiValue = NO_SHIFT;
	vco3340B.semitones = midiValue;
}


void setVco3340BDetune(uint8_t value){
	vco3340B.detune = value;
}

/**
 *  write the current midi note to the VCO control voltage through the appropriate DAC
 *  using the current calibration table.
 */
void updateVco3340BFreq(){
	int dacLvl = midiToVCO3340BCV[midiNote.note + 12 * (vco3340B.octave-CENTER_OCTAVE) + (vco3340B.semitones-NO_SHIFT)] + vco3340B.detune - NO_DETUNE;
	dacWrite(dacLvl, DAC_VCO_3340B_FREQ);
}

void setVco3340BPWMDuty(uint8_t midiValue){
	vco3340B.pwm = midiValue;
}

void updateVco3340BPWMDuty(){
	dacWrite((int) (MAX_VCO3340B_PWM_DUTY * vco3340B.pwm/127.), DAC_VCO_3340B_PWM_DUTY);
}

void setVco3340BPulseLevel(uint8_t midiValue){
	vco3340B.pulse_level = midiValue;
}

void updateVco3340BPulseLevel(){
	dacWrite((int)(MAX_VCO3340B_PULSE_LEVEL * (127.0-vco3340B.pulse_level)/127.0), DAC_V2140D_3340B_PULSE_LVL);
}


void setVco3340BSawLevel(uint8_t midiValue){
	vco3340B.saw_level = midiValue;
}

void updateVco3340BSawLevel(){
	dacWrite((int)(MAX_VCO3340B_SAW_LEVEL * (127.0-vco3340B.saw_level) /127.0), DAC_V2140D_3340B_SAW_LVL);
}

void setVco3340BTriLevel(uint8_t midiValue){
	vco3340B.tri_level = midiValue;
}

void updateVco3340BTriLevel(){
	dacWrite((int)(MAX_VCO3340B_TRI_LEVEL * (127.0-vco3340B.tri_level) /127.0), DAC_V2140D_3340B_TRI_LVL);
}

// ----------------------- hardware test ---------------

// play a range from the lowest to the highest possible note
// (VCO must be connected to the DAC board)
void testScale(){

	midiNote.note  = 24;
	setVco3340BPWMDuty(100);
	while(1){
		//updateVco3340AFreq();
		updateVco3340BFreq();
		//updateVco13700Freq();
		HAL_Delay(200); // 200ms
		toggleBlueLED();
		printf("%d\n", midiNote.note++);
		if (midiNote.note > 95) midiNote.note=24;
	}
}
