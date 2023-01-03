/*
 * vco.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 *
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

#include <synth.h>
#include "ad5644.h"
#include "bh2221.h"
#include "vco.h"
#include "leds.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "midi.h"
#include "stdio.h"
#include "leds.h"

/* External variables --------------------------------------------------------*/

extern MidiNote_t midiNote;

/* Private variables ---------------------------------------------------------*/

static Vco3340Parameters_t vco3340A = {
		.detune=NO_DETUNE,
		.semitones=NO_SHIFT,
		.octave=CENTER_OCTAVE,
		.pwm=DEF_MIDICC_VCO3340_PWM_DUTY
};

static Vco3340Parameters_t vco3340B = {
		.detune=NO_DETUNE,
		.semitones=NO_SHIFT,
		.octave=CENTER_OCTAVE,
		.pwm=DEF_MIDICC_VCO3340_PWM_DUTY
};


// an array that maps a MIDI note to a DAC value for the VCO DAC (from 0 to 4095): e.g. note 36 maps to 442
// this array is normally filled by the calibration process!
// (diff between notes is b/w 54 and 59)

uint32_t note_To_VCO3340A_CV[128] = {
		0,100,200,300,400,500,600,700,800,900,1000,1100,
		1200,1300,1400,1500,1600,1700,1800,1900,
		2000,2100,2200,2300,2400,2500,2600,2700,
		2800,2900,3000,3100,3200,3300,3400,3500,
		3600,3700,3800,3900,4000,4100,4200,4300,
		4400,4500,4600,4700,4800,4900,5000,5100,
		5200,5300,5400,5500,5600,5700,5800,5900,
		6000,6100,6200,6300,6400,6500,6600,6700,
		6800,6900,7000,7100,7200,7300,7400,7500,
		7600,7700,7800,7900,8000,8100,8200,8300,
		8400,8500,8600,8700,8800,8900,9000,9100,
		9200,9300,9400,9500,9600,9700,9800,9900,
		10000,10100,10200,10300,10400,10500,
		10600,10700,10800,10900,11000,11100,11200,11300,
		11400,11500,11600,11700,11800,11900,12000,12100,
		12200,12300,12400,12500,12600,12700};

uint32_t  note_To_VCO3340B_CV[128] = {0,100,200,300,400,500,600,700,800,900,1000,1100,
		1200,1300,1400,1500,1600,1700,1800,1900,
		2000,2100,2200,2300,2400,2500,2600,2700,
		2800,2900,3000,3100,3200,3300,3400,3500,
		3600,3700,3800,3900,4000,4100,4200,4300,
		4400,4500,4600,4700,4800,4900,5000,5100,
		5200,5300,5400,5500,5600,5700,5800,5900,
		6000,6100,6200,6300,6400,6500,6600,6700,
		6800,6900,7000,7100,7200,7300,7400,7500,
		7600,7700,7800,7900,8000,8100,8200,8300,
		8400,8500,8600,8700,8800,8900,9000,9100,
		9200,9300,9400,9500,9600,9700,9800,9900,
		10000,10100,10200,10300,10400,10500,
		10600,10700,10800,10900,11000,11100,11200,11300,
		11400,11500,11600,11700,11800,11900,12000,12100,
		12200,12300,12400,12500,12600,12700};



/* Private function prototypes -----------------------------------------------*/


/* User code -----------------------------------------------*/

/*
 * mcp23017_Tx_GpioA_Buffer_Dma(), mcp23017_Tx_GpioB_Buffer_Dma() and ad5391_Write_Dma() should be called after initVco()
 * to transfer buffer data to devices.
 */
void vcoInit(){

	// TODO mcp23017_Reset_Vco_Sync();

	midiNote.note = DEF_MIDI_NOTE;
	vcoWrite3340AFreqToDac();
	vcoWrite3340BFreqToDac();

	vcoSet3340APWMDuty(DEF_MIDICC_VCO3340_PWM_DUTY); vcoWrite3340APWMDutyToDac();
	vcoSet3340BPWMDuty(DEF_MIDICC_VCO3340_PWM_DUTY); vcoWrite3340BPWMDutyToDac();

	// mute unused mixers:
	bh2221WriteAsync(255, BH2221_V2140D_3340B_SUB_LVL);
	// TODO bh2221_Write_Buffer(255, BH2221_V2140D_IN5);
	// TODO bh2221_Write_Buffer(255, BH2221_V2140D_IN6);
	bh2221WriteAsync(255, BH2221_V2140D_XMOD_LVL);



}

// used during calibration:
void vcoPrepareCalibration(){

	// TODO mcp23017_Mute_Vco_Blocking(); // "unplug" 3340A vco output from mixer so as to reduce interference with other VCO
	bh2221WriteBlocking(MAX_VCO3340A_PWM_DUTY, BH2221_VCO_3340A_PWM_DUTY);  // make sure VCO calibration output
	bh2221WriteBlocking(MAX_VCO3340B_PWM_DUTY, BH2221_VCO_3340B_PWM_DUTY);  // is a square (i.e., 50% duty cycle)
}



// ---------------------------------------------------------------------------------------------------
//                       vco 3340A
// ---------------------------------------------------------------------------------------------------

// a midi value of 2 yields the center octave ; a value of 0 leads to -2 octaves ; a value of 4 leads to +2 octaves

void vcoSet3340AOctave(uint8_t midiValue){
	if (midiValue > 4 || midiValue < 0) midiValue = CENTER_OCTAVE;
	vco3340A.octave = midiValue;
}

void vcoSet3340ASemitones(uint8_t midiValue){
	if (midiValue > 22 || midiValue < 0) midiValue = NO_SHIFT;
	vco3340A.semitones = midiValue;
}


void vcoSet3340ADetune(uint8_t value){
	vco3340A.detune = value;
}

/**
 *  write the current midi note to the VCO control voltage through the appropriate DAC
 *  using the current calibration table.
 */
void vcoWrite3340AFreqToDac(){
	int dacLvl = note_To_VCO3340A_CV[midiNote.note + 12 * (vco3340A.octave-CENTER_OCTAVE) + (vco3340A.semitones-NO_SHIFT)] + vco3340A.detune - NO_DETUNE;
	ad5644WriteAsync(dacLvl, AD5644_VCO_3340A_FREQ);
}

void vcoSet3340APWMDuty(uint8_t midiValue){
	vco3340A.pwm = midiValue;
}

void vcoWrite3340APWMDutyToDac(){
	bh2221WriteAsync((int) (MAX_VCO3340A_PWM_DUTY * vco3340A.pwm/127.), BH2221_VCO_3340A_PWM_DUTY);
}

// ---------------------------------------------------------------------------------------------------
//               vco 3340B
// ---------------------------------------------------------------------------------------------------

void vcoSet3340BOctave(uint8_t midiValue){
	if (midiValue > 4 || midiValue < 0) midiValue = CENTER_OCTAVE;
	vco3340B.octave = midiValue;
}

void vcoSet3340BSemitones(uint8_t midiValue){
	if (midiValue > 22 || midiValue < 0) midiValue = NO_SHIFT;
	vco3340B.semitones = midiValue;
}


void vcoSet3340BDetune(uint8_t value){
	vco3340B.detune = value;
}

/**
 *  write the current midi note to the VCO control voltage through the appropriate DAC
 *  using the current calibration table.
 */
void vcoWrite3340BFreqToDac(){
	int dacLvl = note_To_VCO3340B_CV[midiNote.note + 12 * (vco3340B.octave-CENTER_OCTAVE) + (vco3340B.semitones-NO_SHIFT)] + vco3340B.detune - NO_DETUNE;
	ad5644WriteAsync(dacLvl, AD5644_VCO_3340B_FREQ);
}

void vcoSet3340BPWMDuty(uint8_t midiValue){
	vco3340B.pwm = midiValue;
}

void vcoWrite3340BPWMDutyToDac(){
	bh2221WriteAsync((int) (MAX_VCO3340B_PWM_DUTY * vco3340B.pwm/127.), BH2221_VCO_3340B_PWM_DUTY);
}


// ----------------------- hardware test ---------------

// play a range from the lowest to the highest possible note
void vcoTestScale(){

	midiNote.note  = 24;
	vcoSet3340BPWMDuty(100);
	while(1){
		//updateVco3340AFreq();
		vcoWrite3340BFreqToDac();
		//updateVco13700Freq();
		HAL_Delay(200); // 200ms
		ledToggle(LED_RED);
		printf("%d\n", midiNote.note++);
		if (midiNote.note > 95) midiNote.note=24;
	}
}

