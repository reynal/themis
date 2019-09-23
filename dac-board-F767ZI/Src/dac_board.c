/*
 * dac_board.c
 *
 *  Created on: Oct 2, 2018
 *      Author: reynal
 *
 *
 * ----------------- mixers ----------------
 * V2164D : -33mV/dB => with a 2V input range, this yields 60dB of attenuation.
 *
 * There are two mixers with four inputs each. Inputs are numbered from 1 to 8 :
 *
 * in1 : VCO 3340 (post switch)
 * in2 : VCO 13700 tri
 * in3 : VCO 13700 square
 * in4 : VCO 13700 subbass
 * in5 : FM
 * in6 : RING MODULATOR
 * in7 : S/H
 * in8 : VCA (separate channel that is fed by the output of the VCF)
 *
 * ----------------- switches ----------------
 * There are several analog switches:
 * - 3340 : the DG411 makes it possible to chose between TRI, SAW and SQU output ; the last switch of the IC can enable the VCO synchronisation 13700 -> 3340.
 * - 3320 : the switch can select 2nd or 4th order lowpass filter
 *
 *
 * ----------------- 3320 vcf ----------------
 * V3320 : 60mV/decade => from 20Hz to 2kHz = 120mV ; we have a 0.1 attenuator between the DAC and the V3320 CV input => 1.2V.
 *         Hence with 2V range at the DAC output, we can sweep frequency over a bit more than 3 decades!
 *         Finally, since there's an exp converter inside the 3320, kbd_tracking reduces to simply translating the input voltage!
 *
 * ----------------- enveloppes features ----------------
 *
 * - on most synths env's go from 1ms to 10" (see Moog Little phatty for instance).
 * - cutoff usually goes from 20Hz to 16kHz
 *
 * ----------------- vca env ----------------
 * VCA ADSR enveloppe : the tricky thing here is how velocity influences the
 * enveloppe amplitude. The following formula gives the global multiplicative factor:
 *
 *        tmp_velocity_mul_factor = (1-velocity_sensitivity) + (velocity/127.)*velocity_sensitivity;
 *
 * (we make use of a tmp var to speed up calculation, which gets updated once every NOTE ON event)
 *
 *
 * ----------------- vcf env ----------------
 * VCF ADSR enveloppe: here again things become quiet tricky when it comes to computing levels!!! (timing are easy to understand in comparison)
 * - env starts from initial value "vcf_cutoff" (see globa_synth_params_t)
 * - env raises during attack_time to a maximum value defined as
 *   max_level = env_amount * tmp_velocity_mul_factor
 * - env decays to sustain lvl = max_level * adsr.sustain_lvl
 *
 * In addition, the whole enveloppe is scaled up or down ( that is, including initial level)
 * depending on the current note being played AND according to kbd_tracking.
 * We thus define:
 *
 * tmp_kbdtracking_mul_factory = (note - 64)/64.0 *  kbd_tracking;
 *
 *   Note : tmp_velocity_mul_factor is defined like for the VCA env.
 *
 */

#include "stm32f7xx_hal.h"
#include "dac_board.h"
#include "math.h"
#include "midi.h"
#include "adsr.h"
//#include "stdlib.h
#include "vco_calibration.h"
#include "main.h"
#include "stdio.h"

/* Private variables ---------------------------------------------------------*/

// ------------- SPI --------------
uint8_t txSpiDacsBuff[2]; // channel A then channel B for SPI_dac
uint8_t rxSpiMidiBuff[3]; // RX BUFF for SPI_rpi coming from RPi (three MIDI bytes)

// ------------ wavetable generation -------
int waveTableCounter=0; // wavetable generation
#define WAVE_TABLE_LEN 50
uint16_t waveTable[WAVE_TABLE_LEN];

// ------------- for debugging purpose ---------
int blueButtonFlag = 1; //
int testCounter=0; // pour int EXTI
int demoMode=0; // active if 1
int demoCounter=0; // time counter
#define DEMO_NOTES_LEN 4
int demoNotes[] = {36, 48, 36, 48}; // de 36 a 71
int demoNoteCounter = 0;


// ------------ adsr enveloppes -----------
int adsrInterruptCounter=0;

AdsrParams vcaAdsr = {
		.attackTimeMs = MAX_ATTACK_TIME_VCA * DEF_MIDICC_ATTACK_TIME_VCA / 127.0,
		.decayTimeMs = MAX_DECAY_TIME_VCA * DEF_MIDICC_DECAY_TIME_VCA / 127.0,
		.releaseTimeMs = MAX_RELEASE_TIME_VCA * DEF_MIDICC_RELEASE_TIME_VCA / 127.0,
		.sustainLevel = MAX_SUSTAIN_LVL_VCA * DEF_MIDICC_SUSTAIN_LVL_VCA / 127.0,
};
StateMachineVca stateMachineVca = {
		//.t = 0,
		.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * DEF_MIDICC_VELOCITY_SENSITIVITY_VCA / 127.0,
		//.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_ATTACK_TIME),
		//.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_DECAY_TIME),
		//.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_RELEASE_TIME),
		.machineState=IDLE,
		.adsrParam = &vcaAdsr
};
AdsrParams vcfAdsr = {
		.attackTimeMs = MAX_ATTACK_TIME_VCF * DEF_MIDICC_ATTACK_TIME_VCF / 127.0,
		.decayTimeMs = MAX_DECAY_TIME_VCF * DEF_MIDICC_DECAY_TIME_VCF / 127.0,
		.releaseTimeMs = MAX_RELEASE_TIME_VCF * DEF_MIDICC_RELEASE_TIME_VCF / 127.0,
		.sustainLevel = MAX_SUSTAIN_LVL_VCF * DEF_MIDICC_SUSTAIN_LVL_VCF / 127.0,
};
StateMachineVcf stateMachineVcf = {
		.t = 0,
		.tMax=0,
		.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * DEF_MIDICC_VELOCITY_SENSITIVITY_VCF / 127.0,
		.kbdTracking = MAX_KBD_TRACKING_VCF * DEF_MIDICC_KBD_TRACKING_VCF / 127.0,
		.envAmount = MAX_ENV_AMOUNT_VCF * DEF_MIDICC_ENV_AMOUNT_VCF / 127.0,
		//.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.attackTimeMs),
		//.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.decayTimeMs),
		//.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.releaseTimeMs),
		.machineState=IDLE,
		.adsrParam = &vcfAdsr
};
GlobalSynthParams globalParams = {
		.vcfCutoff = MAX_CUTOFF * DEF_MIDICC_CUTOFF / 127.0,
		.vcfResonance=MAX_RESONANCE * DEF_MIDICC_RESONANCE / 127.0
};

// converts a MIDI CC data1 value to the corresponding enum constant in MidiCCParam
MidiCCParam midiccCodeToParams[128] = {
		UNUSED_CC, 		// 0
		UNUSED_CC, 		// 1
		UNUSED_CC, 		// 2
		UNUSED_CC, 		// 3
		UNUSED_CC, 		// 4
		UNUSED_CC, 		// 5
		UNUSED_CC, 		// 6
		UNUSED_CC, 		// 7
		UNUSED_CC, 		// 8
		WAVE_3340, 		// 9
		DETUNE_13700, 	// 10
		WAVE_13700, 	// 11
		UNUSED_CC, 		// 12
		UNUSED_CC, 		// 13
		PWM_3340, 		// 14
		LEVEL_3340, 	// 15
		LEVEL_13700,	// 16
		UNUSED_CC, 		// 17
		UNUSED_CC, 		// 18
		VCF_CUTOFF, 	// 19 (MSB)
		UNUSED_CC, 		// 20
		VCF_RESONANCE,	// 21
		VCF_KBDTRACKING,// 22
		VCF_ATTACK,		// 23
		VCF_DECAY,		// 24
		VCF_SUSTAIN,	// 25
		VCF_RELEASE,	// 26
		VCF_EG,			// 27
		VCA_ATTACK,		// 28
		VCA_DECAY,		// 29
		VCA_SUSTAIN,	// 30
		VCA_RELEASE,	// 31
		UNUSED_CC, 		// 32
		UNUSED_CC, 		// 33
		UNUSED_CC, 		// 34
		UNUSED_CC, 		// 35
		UNUSED_CC, 		// 36
		UNUSED_CC, 		// 37
		UNUSED_CC, 		// 38
		UNUSED_CC, 		// 39
		UNUSED_CC, 		// 40
		UNUSED_CC, 		// 41
		UNUSED_CC, 		// 42
		UNUSED_CC, 		// 43
		UNUSED_CC, 		// 44
		UNUSED_CC, 		// 45
		UNUSED_CC, 		// 46
		UNUSED_CC, 		// 47
		UNUSED_CC, 		// 48
		UNUSED_CC, 		// 49
		UNUSED_CC, 		// 50
		UNUSED_CC, 		// 51 (cut off LSB)
		UNUSED_CC, 		// 52
		UNUSED_CC, 		// 53
		UNUSED_CC, 		// 54
		UNUSED_CC, 		// 55
		UNUSED_CC, 		// 56
		UNUSED_CC, 		// 57
		UNUSED_CC, 		// 58
		UNUSED_CC, 		// 59
		UNUSED_CC, 		// 60
		UNUSED_CC, 		// 61
		UNUSED_CC, 		// 62
		UNUSED_CC, 		// 63
		UNUSED_CC, 		// 64
		UNUSED_CC, 		// 65
		UNUSED_CC, 		// 66
		UNUSED_CC, 		// 67
		UNUSED_CC, 		// 68
		UNUSED_CC, 		// 69
		UNUSED_CC, 		// 70
		UNUSED_CC, 		// 71
		UNUSED_CC, 		// 72
		UNUSED_CC, 		// 73
		OCTAVE_3340, 	// 74
		OCTAVE_13700, 		// 75
		UNUSED_CC, 		// 76
		SYNC_3340, 		// 77
		UNUSED_CC, 		// 78
		UNUSED_CC, 		// 79
		UNUSED_CC, 		// 80
		UNUSED_CC, 		// 81
		UNUSED_CC, 		// 82
		UNUSED_CC, 		// 83
		UNUSED_CC, 		// 84
		UNUSED_CC, 		// 85
		UNUSED_CC, 		// 86
		UNUSED_CC, 		// 87
		UNUSED_CC, 		// 88
		UNUSED_CC, 		// 89
		UNUSED_CC, 		// 90
		UNUSED_CC, 		// 91
		UNUSED_CC, 		// 92
		UNUSED_CC, 		// 93
		UNUSED_CC, 		// 94
		UNUSED_CC, 		// 95
		UNUSED_CC, 		// 96
		UNUSED_CC, 		// 97
		UNUSED_CC, 		// 98
		UNUSED_CC, 		// 99
		UNUSED_CC, 		// 100
		UNUSED_CC, 		// 101
		UNUSED_CC, 		// 102
		UNUSED_CC, 		// 103
		VCA_VELOCITY_SENSITIVITY, // 104
		UNUSED_CC, 		// 105
		UNUSED_CC, 		// 106
		UNUSED_CC, 		// 107
		UNUSED_CC, 		// 108
		VCF_ORDER, 		// 109
		VCF_VELOCITY_SENSITIVITY, // 110
		UNUSED_CC, 		// 111
		UNUSED_CC, 		// 112
		UNUSED_CC, 		// 113
		UNUSED_CC, 		// 114
		UNUSED_CC, 		// 115
		UNUSED_CC, 		// 116
		UNUSED_CC, 		// 117
		UNUSED_CC, 		// 118
		UNUSED_CC, 		// 119
		UNUSED_CC, 		// 120
		UNUSED_CC, 		// 121
		UNUSED_CC, 		// 122
		UNUSED_CC, 		// 123
		UNUSED_CC, 		// 124
		UNUSED_CC, 		// 125
		UNUSED_CC, 		// 126
		CALIBRATE, 		// 127
};


// ------------- vco's ---------------------
VcoParameters vco13700 = { .detune=0.0, .octave=0};
VcoParameters vco3340 = { .detune=0.0, .octave=0};

// ------------- drums ----------------------
DrumTriggers drumTriggers = { .bassdrumCounter=0, .rimshotCounter=0, .snareCounter=0, .lowtomCounter=0, .hightomCounter=0};

// ---------------- midi  ----------------
MidiNote midiNote = { .note = 60, .velocity = 100}; // saves the note inside the last midi message
// midi_receiver_state_t midi_receiver_state; // midi state-machine state
// uint8_t midi_byte1, midi_byte2, midi_byte3; // tmp vars for midi state-machine

/* External variables --------------------------------------------------------*/

extern UART_HandleTypeDef *huartSTlink;
extern uint8_t rxUartSTlinkBuff[3];
extern TIM_HandleTypeDef *htimEnveloppes, *htimCalib;
extern SPI_HandleTypeDef *hspiDacs;

// an array that maps a MIDI note to a DAC value for the VCO DAC (from 0 to 4095): e.g. note 36 maps to 442
// this array is normally filled by the calibration process!
int midiToVCO3340CV[128] = {
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
		 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095};


int midiToVCO13700CV[128] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0-11
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 12-23
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 24-35
		 442, 483, 525, 570, 612, 657, 701, 745, 787, 833, 874, 918, // 36-47
		 961, 1003, 1048, 1091, 1134, 1179, 1222, 1265, 1311, 1357, 1396, 1441, // 48-59
		 1486, 1530, 1575, 1613, 1658, 1703, 1746, 1792, 1838, 1881, 1917, 1963, // 60-71
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 72-83
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 84-95
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96-107
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 108-119
		 0, 0, 0, 0, 0, 0, 0, 0};


// --------------------------------------------------------------------------------------------------
//                                     low-level hardware code
// --------------------------------------------------------------------------------------------------


/**
 * sends the appropriate address to the 74LS128 3-to-8 decoder
 * so that the corresponding MCP4822 chip is activated when CS is asserted later
 */
void dacSelect(int chipNumber){

	// selection du CNA: A0=PC4, A1=PB1, A2=PC5 and address=A2.A1.A0
	chipNumber &= 0x07;

	switch (chipNumber){
	case 0:
		//GPIOC->BSRR = (1<<(16+2)); // reset A2=PC2
		//GPIOB->BSRR = (1<<(16+11)); // reset A1=PB11
		//GPIOC->BSRR = (1<<(16)); // reset A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 1:
		//GPIOC->BSRR = (1<<(16+2)); // reset A2=PC2
		//GPIOB->BSRR = (1<<(16+11)); // reset A1=PB11
		//GPIOC->BSRR = (1<<(0)); 	  // set A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	case 2:
		//GPIOC->BSRR = (1<<(16+2)); // reset A2=PC2
		//GPIOB->BSRR = (1<<(11));    // set A1=PB11
		//GPIOC->BSRR = (1<<(16)); // reset A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 3:
		//GPIOC->BSRR = (1<<(16+2)); // reset A2=PC2
		//GPIOB->BSRR = (1<<(11));    // set A1=PB11
		//GPIOC->BSRR = (1<<(0));    // set A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	case 4:
		//GPIOC->BSRR = (1<<(2));    // set A2=PC2
		//GPIOB->BSRR = (1<<(16+11)); // reset A1=PB11
		//GPIOC->BSRR = (1<<(16)); // reset A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 5:
		//GPIOC->BSRR = (1<<(2));    // set A2=PC2
		//GPIOB->BSRR = (1<<(16+11)); // reset A1=PB11
		//GPIOC->BSRR = (1<<(0)); 	  // set A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	case 6:
		//GPIOC->BSRR = (1<<(2));    // set A2=PC2
		//GPIOB->BSRR = (1<<(11));    // set A1=PB11
		//GPIOC->BSRR = (1<<(16)); // reset A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 7:
		//GPIOC->BSRR = (1<<(2));    // set A2=PC2
		//GPIOB->BSRR = (1<<(11));    // set A1=PB11
		//GPIOC->BSRR = (1<<(0));    // set A0=PC0
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	}
}

/**
 * Write a sample to one of eight possible MCP4822 DAC through the SPI2 bus
 * @param word12bits an unsigned 12 bits word
 * @param one of 8 possible physical chips
 * @param 4822 dac channel MCP4822_CHANNEL_A or MCP4822_CHANNEL_B or MCP4822_CHANNEL_A_GAIN2 or MCP4822_CHANNEL_B_GAIN2 if you want a 0-4096mV range
 */

void dac4822ABWrite(int word12bits, int chip, int channelAB){

	if (word12bits < 0) word12bits = 0;
	word12bits &= 0xFFF;

	// selection du CNA:
	dacSelect(chip);

	txSpiDacsBuff[0]=(((word12bits >> 8) & 0x0F)) | channelAB;
	txSpiDacsBuff[1]=(word12bits & 0xFF);

	//GPIOB->BSRR = (1<<(16+1)); // PB1 = 0 (=input ENABLE of 74LS138, which indirectly selects the corresponding circuit)
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(hspiDacs, txSpiDacsBuff, 2);

}


/**
 * Write the given word to the given DAC
 */
void dacWrite(int word12bits, Dac targetDac){

	switch (targetDac){
		case DAC_VCO_13700:
			dac4822ABWrite(word12bits, 0,  MCP4822_CHANNEL_A_GAIN2);
			break;

		case DAC_NOISE:
			dac4822ABWrite(word12bits, 0, MCP4822_CHANNEL_B);
			break;

		case DAC_V2140D_13700_SQU_LVL:
		case DAC_V2140D_IN3 :
			dac4822ABWrite(word12bits, 1, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_13700_SUBBASS_LVL:
		case DAC_V2140D_IN4 :
			dac4822ABWrite(word12bits, 1, MCP4822_CHANNEL_B);
			break;

		case DAC_V2140D_3340_LVL:
		case DAC_V2140D_IN1 :
			dac4822ABWrite(word12bits, 2, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_13700_TRI_LVL:
		case DAC_V2140D_IN2 :
			dac4822ABWrite(word12bits, 2, MCP4822_CHANNEL_B);
			break;

		case DAC_VCO_3340_FREQ :
			// debug syd if (word12bits > VCO3340_MAX_INPUT_CV) word12bits = VCO3340_MAX_INPUT_CV;
			dac4822ABWrite(word12bits, 3, MCP4822_CHANNEL_A_GAIN2);
			break;

		case DAC_VCO_3340_PWM_DUTY :
			dac4822ABWrite(word12bits, 3, MCP4822_CHANNEL_B_GAIN2);
			break;

		case DAC_V2140D_SH_LVL:
		case DAC_V2140D_IN7 : // GAIN3_B sur schema EAGLE
			dac4822ABWrite(word12bits, 4, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_VCA :
		case DAC_V2140D_IN8:  // GAIN4_B sur schema EAGLE
			dac4822ABWrite(word12bits, 4, MCP4822_CHANNEL_B);
			break;

		case DAC_V2140D_FM_LVL:
		case DAC_V2140D_IN5 : // GAIN1_B sur schema EAGLE
			dac4822ABWrite(word12bits, 5, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_RINGMOD_LVL:
		case DAC_V2140D_IN6 : // GAIN2_B sur schema EAGLE
			dac4822ABWrite(word12bits, 5, MCP4822_CHANNEL_B);
			break;

		case DAC_VCF_CUTOFF:
			dac4822ABWrite(word12bits, 6, MCP4822_CHANNEL_A);
			break;

		case DAC_VCF_RES :
			dac4822ABWrite(word12bits, 6, MCP4822_CHANNEL_B);
			break;

		case DAC_EN_RABE_A:
			dac4822ABWrite(word12bits, 7, MCP4822_CHANNEL_A);
			break;

		case DAC_EN_RABE_B :
			dac4822ABWrite(word12bits, 7, MCP4822_CHANNEL_B);
			break;

	}
}


/**
 *  write the given amplitude to the VCA control voltage through the appropriate DAC
 *  @param amplitude must be b/w 0 and 1
 */
void dacVcaWrite(double amplitude){
	if (amplitude < 0.0) amplitude = 0;
	else if (amplitude > 1.0) amplitude = 1.0;

	int i = (int)((1.0-amplitude) * 4095); // 1-amplitude => because we have OpAmp inverters somewhere in the path!
	//int i = (int)(amplitude * 4095); // DEBUG
	dacWrite(i, DAC_V2140D_VCA);
	//dacWrite(4095, DAC_VCA);
}

/**
 *  write the given cutoff frequency to the VCF control voltage through the appropriate DAC
 *  @param cutoff frequency value must be b/w 0 (min) and 1 (max)
 */
void dacVcfCutoffWrite(double cutoff){

	if (cutoff > 1.0) cutoff = 1.0;
	else if (cutoff < 0.0) cutoff = 0.0;
	int i = (int)((1.0-cutoff) * 4095);
	//int i = (int)(cutoff * 4095); // DEBUG
	// TODO : actual cutoff CV is GLOBAL CUTOFF + ENVELOPPE * EG_DEPTH parameter
	dacWrite(i, DAC_VCF_CUTOFF);
}


// --------------------------------------------------------------------------------------------------
//                                     hardware test code
// --------------------------------------------------------------------------------------------------

// test if LS138 decoding works properly (check signals with oscilloscope and LS138 unplugged from support)
void testDacSelect(){

	int dac;
	while(1){
		for(dac=0; dac < 8; dac++){
			dacSelect(dac);
			HAL_Delay(1);
		}
		HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
	}
}

// test if SPI5 (DAC's) works properly (check signals with oscilloscope and LS138 now plugged in support)
void testDacWriteSPI(){

	int word12bits;
	int dac=0;
	int i=0;
	while(1){
		//dac4822ABWrite(word12bits, 3, MCP4822_CHANNEL_B);
		//dacWrite(word12bits, DAC_VCO_3340_FREQ);
		//dacWrite(word12bits, DAC_VCO_13700);
		for(word12bits=0; word12bits < 4096; word12bits+=16){
			for(dac=0; dac < 8; dac++){
				//dacWrite(word12bits, DAC_VCO_3340_FREQ);
				dac4822ABWrite(word12bits, dac, MCP4822_CHANNEL_A);
				HAL_Delay(1);
				dac4822ABWrite(word12bits, dac, MCP4822_CHANNEL_B);
				HAL_Delay(1);
			}

		}
		HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
		printf("DAC test %d\n", i++);
	}
}

// --------------------------------------------------------------------------------------------------
//                                     HAL interrupt handlers
// --------------------------------------------------------------------------------------------------

/**
 * Callback implementation for the SPI peripheral "end of transfer" interruption
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi ){
	//__NOP();
	if (hspi == hspiDacs){ // on check que c'est le bon bus SPI (y en a plusieurs !)
		//GPIOB->BSRR = (1<<1); // PB1 = SPI2_CS = 1 (on remonte CS du DAC ça sert à valider la sortie analo puisque par ailleurs LDAC=0)
		HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);
	}
}

/**
 * Callback for the TIMER 1 "end of period" interruption
 * Currently PERIOD = 1/20kHz = 50us
 * - every 50us we push a new sample to the DIG VCO dac (sample freq = 20kHz)
 * - every 1ms = 20 * 50us (i.e. every 20 calls) we push a new sample to all other enveloppes DAC (1kHz sample freq)
 * BUT since after each SPI bus write we must wait (around 16 * 1/3MHz = 5us) for the transfer to complete before writing a new word,
 * it'd be a waste of time... so there's a clever trick that consists in writing one distinct enveloppe at each timer call
 * (since there are 20 timer calls b/w every enveloppe update, we could update up to 20 distinct enveloppes this way!
 * of course there are only 15 available DAC's on the board, so we do nothing during the last 5 calls anyway)
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if (htim == htimEnveloppes){ // on check que c'est le bon TIMER

		//HAL_GPIO_WritePin(GPIOB, LD3_Pin, demoMode == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);


		//HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
		//testCounter+=10;
		//if (testCounter > 4000) testCounter = 0;
		//dacWrite(testCounter, DAC_VCO_3340_FREQ);

		// always write one WAVETABLE sample
		//waveTableCounter++;
		//if (waveTableCounter == WAVE_TABLE_LEN) waveTableCounter = 0;
		//dacWrite(wave_table[waveTableCounter], DAC_VCO_DIG);

		// white noise for the drum machine :
		/* debug 2/4/19 int noise = (int)(4096.0 * rand() / RAND_MAX);
		dacWrite(noise, DAC_NOISE);*/
		// syd 8/9/19 updateDrumMachine();

		// once every ADSR_TIMER_PERIOD, compute then write all ADSR enveloppes + VCO CV's,
		// yet see trick above in this function documentation
		adsrInterruptCounter++;
		if (adsrInterruptCounter == ADSR_TIMER_PERIOD_FACTOR) {
			adsrInterruptCounter = 0;
			//if (demoMode==1) playDemo();
		}

		// ALWAYS update VCO *before* VCA so that we won't hear the note jump
		switch (adsrInterruptCounter){
			case 0:
				// first convert midi note to appropriate DAC voltage according to calibration:
				//midiToVCO13700CV[midiNote.note];
				//dacWrite(midiToVCO13700CV[midiNote.note + 12 * (octave_mul_13700 - 1)], DAC_VCO_13700);
				dacWrite(midiToVCO13700CV[midiNote.note], DAC_VCO_13700);
				//dacWrite(testCounter, DAC_VCO_3340_FREQ);
				//dacWrite(testCounter, DAC_VCF_CUTOFF);
				break;
			case 1:
				//dac = midiToVCO3340CV[midiNote.note];
				//dacWrite(midiToVCO3340CV[midi_note.note + 12 * (octave_mul_3340 - 1)], DAC_VCO_3340_FREQ);
				dacWrite(midiToVCO3340CV[midiNote.note], DAC_VCO_3340_FREQ);
				//dacWrite(2000, DAC_VCO_3340_FREQ);
				break;
			case 2:
				updateVCAEnveloppeStateMachine();
				break;
			case 3:
				updateVCFEnveloppeStateMachine();

				break;

			// todo : mixers, etc
		}
	}
	//else if (htim == htimCalib) HAL_GPIO_TogglePin(SAW_3340_GPIO_Port, SAW_3340_Pin);
}

/**
 *  Called every time level changes on pin GPIO_Pin
 *  We "listen" to two pins:
 *  - CE_RPi_Pin: this pin is connected to the CE ("chip enable") signal of the Raspberry Pi ; this signal
 *  			gets low every time data is to be transmitted by the Raspberry Pi to the STM32 through the SPI3 bus
 *  - USER_Btn_Pin: this pin is connected to the blue button on the STM32 board; can be used for any kind of debugging purpose
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	// RPi as note trigger
	if (GPIO_Pin == CE_RPi_Pin){ // CE0 from RaspBerryPi (PC6)

		//testCounter++;
		//HAL_SPI_Receive(hspiMidi,rx_spiMidi_buff,3,HAL_MAX_DELAY); // read three MIDI bytes in a row //A REACTIVER SI BRANCHE SUR RPI //DEBUG
		//testCounter = rx_spi3_buff[0];
		//midiFromSpiMessageHandler(rx_spi3_buff[0]);
		// debug syd 6/4/19 processIncomingMidiMessage(rx_spiMidi_buff[0],rx_spiMidi_buff[1],rx_spiMidi_buff[2]);
	}

	// button blue as note trigger
	else if (GPIO_Pin == USER_Btn_Pin){ // PC13

	  blueButtonFlag = !blueButtonFlag;
		//blueButtonFlag = HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin);
	  //demoMode = blueButtonFlag;
	  //HAL_GPIO_WritePin(GPIOB, LD3_Pin, demoMode == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);

	  /*if (blueButtonFlag == 1){ // note ON
		  //processIncomingMidiMessage(NOTE_ON|0xA, 36,100); // drum debug ???
		  //processIncomingMidiMessage(NOTE_ON, 36,100);
		  demoMode = 1;
	  }
	  else if (blueButtonFlag == 0){ // note OFF
		  //processIncomingMidiMessage(NOTE_OFF, 50, 100);
		  demoMode = 0;
	  }*/
	}
}

/**
 * Callback for the UART peripheral receive data process
 * Called when a given amount of data has been received on given UART port
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	//__NOP();//test if we reach this position
	//printf("Received: %s\n", rxUartSTlinkBuff);
	processIncomingMidiMessage(rxUartSTlinkBuff[0],rxUartSTlinkBuff[1],rxUartSTlinkBuff[2]);
	HAL_UART_Receive_IT(huartSTlink, rxUartSTlinkBuff, 3); // wait for next MIDI msg
}




// --------------------------------------------------------------------------------------------------
//                                     ADSR, drums and MIDI code
// --------------------------------------------------------------------------------------------------


/**
 * Initialize synth parameters and DAC values
 */
void initSynthParams(){

	// wavetable init:
	//int i;
	//for (i=0; i<WAVE_TABLE_LEN; i++) waveTable[i] = 1000 * (1.0 + sin(0.0628 * i));

	dacVcaWrite(0.0); // makes sure we don't hear anything
	HAL_Delay(1); // wait 1ms for transfer to complete (could be lower but HAL_Delay can't go below)
	dacVcfCutoffWrite(0.0); // makes sure filter is off
	HAL_Delay(1);

	dacWrite(2428, DAC_VCO_3340_FREQ); // outputs A4 @ 440Hz
	HAL_Delay(1);
	setMidiCCParam(PWM_3340, DEF_MIDICC_VCO3340_PWM_DUTY);
	HAL_Delay(1);
	setMidiCCParam(WAVE_3340, 0);
	setMidiCCParam(SYNC_3340, 0);
	setMidiCCParam(VCF_ORDER, 0);

}


/**
 * Updates the state machines associated with the generation of the VCA ADSR enveloppes, then write it to the appropriate DAC
 * This method should be called from the timer handler (every ms or so)
 */
void updateVCAEnveloppeStateMachine(){

	switch (stateMachineVca.machineState){

			case IDLE:
				break;

			case ATTACK:
				dacVcaWrite(stateMachineVca.amplitude);
				stateMachineVca.amplitude += stateMachineVca.tmpDelta;
				//stateMachineVca.t++;
				if (stateMachineVca.amplitude >= stateMachineVca.tmpTargetLevel){
					// prepare dyn params for DECAY phase:
					stateMachineVca.tmpTargetLevel *= vcaAdsr.sustainLevel; // modulate sustain level with velocity factor
					stateMachineVca.tmpDelta = ADSR_TIMER_PERIOD_MS * (stateMachineVca.tmpTargetLevel-stateMachineVca.amplitude) / (vcaAdsr.decayTimeMs); // prepare dx for the attack phase of x(t)
					stateMachineVca.machineState = DECAY;
				}
				break;

			case DECAY:

				if (stateMachineVca.amplitude > stateMachineVca.tmpTargetLevel) {
					stateMachineVca.amplitude += stateMachineVca.tmpDelta;
					dacVcaWrite(stateMachineVca.amplitude);
				}
				// else stays on sustain plateau until NOTE OFF occurs
				break;

			case RELEASE :

				if (stateMachineVca.amplitude > 0.0) { // stateMachineVca.tmpTargetLevel) {
					stateMachineVca.amplitude += stateMachineVca.tmpDelta; // else stays on sustain plateau until NOTE OFF occurs
					dacVcaWrite(stateMachineVca.amplitude);
				}
				else stateMachineVca.machineState = IDLE;
				break;
		}

}


/**
 * Updates the state machines associated with the generation of the VCF ADSR enveloppes,
 * then write it to the appropriate DAC
 */
void updateVCFEnveloppeStateMachine(){

	switch (stateMachineVca.machineState){

			case IDLE:
				break;

			case ATTACK:
				dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency + stateMachineVcf.tmpKbdtrackingShiftFactor);
				stateMachineVcf.cutoffFrequency += stateMachineVcf.tmpDelta;
				stateMachineVcf.t++;
				if (stateMachineVcf.t >= stateMachineVcf.tMax){
					// prepare dyn params for DECAY phase:
					stateMachineVcf.t=0;
					stateMachineVcf.tMax = vcfAdsr.decayTimeMs / (ADSR_TIMER_PERIOD_MS);
					stateMachineVcf.tmpTargetLevel = globalParams.vcfCutoff + (stateMachineVcf.tmpTargetLevel-globalParams.vcfCutoff)*vcfAdsr.sustainLevel; // modulate sustain level with velocity factor
					stateMachineVcf.tmpDelta = (stateMachineVcf.tmpTargetLevel-stateMachineVcf.cutoffFrequency) / (stateMachineVcf.tMax); // prepare dx for the attack phase of x(t)
					stateMachineVcf.machineState = DECAY;
				}
				break;

			case DECAY:
				if (stateMachineVcf.t <= stateMachineVcf.tMax) {
					stateMachineVcf.t++;
					stateMachineVcf.cutoffFrequency += stateMachineVcf.tmpDelta;
					dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency + stateMachineVcf.tmpKbdtrackingShiftFactor);
				}
				// else stays on sustain plateau until NOTE OFF occurs
				break;

			case RELEASE :
				stateMachineVcf.t++;
				if (stateMachineVcf.t <= stateMachineVcf.tMax) {
					stateMachineVcf.cutoffFrequency += stateMachineVcf.tmpDelta; // else stays on sustain plateau until NOTE OFF occurs
					dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency + stateMachineVcf.tmpKbdtrackingShiftFactor);
				}
				else stateMachineVcf.machineState = IDLE;
				break;
		}

}

/**
 *  Prepare the envelope state machines following a MIDI NOTE ONE message
 */
void midiNoteOnHandler(){

	// switch on LED so that we can monitor enveloppe level TODO : pwm !
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);

	// ---------- VCA dyn parameters -----------
	stateMachineVca.amplitude=0.0;
	stateMachineVca.tmpTargetLevel = ((1.0-stateMachineVca.velocitySensitivity) + (midiNote.velocity/127.)*stateMachineVca.velocitySensitivity);
	stateMachineVca.tmpDelta = ADSR_TIMER_PERIOD_MS * stateMachineVca.tmpTargetLevel / vcaAdsr.attackTimeMs; // prepare dx for the attack phase of x(t)

	// ---------- VCF dyn parameters -----------

	double velocityMulFactor = ((1.0-stateMachineVcf.velocitySensitivity) + (midiNote.velocity/127.)*stateMachineVcf.velocitySensitivity);
	stateMachineVcf.t = 0;
	stateMachineVcf.tMax = vcfAdsr.attackTimeMs / (ADSR_TIMER_PERIOD_MS);
	stateMachineVcf.tmpTargetLevel = stateMachineVcf.envAmount * velocityMulFactor ;
	stateMachineVcf.tmpDelta = (stateMachineVcf.tmpTargetLevel-globalParams.vcfCutoff) / stateMachineVcf.tMax; // prepare dx for the A phase of x(t)
	stateMachineVcf.tmpKbdtrackingShiftFactor = (midiNote.note - 64)/64.0 * stateMachineVcf.kbdTracking * MAX_KBD_TRACKING_VCF;
	stateMachineVcf.cutoffFrequency = globalParams.vcfCutoff; // starts at global cutoff value


	// prepare state machines:
	stateMachineVca.machineState=ATTACK; // force vca machine state to ATTACK
	stateMachineVcf.machineState=ATTACK; // force vcf machine state to ATTACK

	//midi_note.note += 10; // DEBUG
	//if (	midi_note.note >= 4095) midi_note.note = 0;
}

/**
 *  Prepare the enveloppes state machines following a MIDI NOTE ONE message
 */
void midiNoteOffHandler(){

	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);

	stateMachineVca.tmpTargetLevel = 0.0;
	stateMachineVca.tmpDelta = - ADSR_TIMER_PERIOD_MS * stateMachineVca.amplitude / vcaAdsr.releaseTimeMs; // prepare dx for the R phase of x(t)

	stateMachineVcf.t=0;
	stateMachineVcf.tMax = vcfAdsr.releaseTimeMs / (ADSR_TIMER_PERIOD_MS);
	stateMachineVcf.tmpTargetLevel = globalParams.vcfCutoff;
	stateMachineVcf.tmpDelta = ADSR_TIMER_PERIOD_MS * (stateMachineVcf.tmpTargetLevel-stateMachineVcf.cutoffFrequency) / stateMachineVcf.tMax; // prepare dx for the R phase of x(t)

	stateMachineVca.machineState=RELEASE; // force vca machine state to RELEASE
	stateMachineVcf.machineState=RELEASE; // force vcf machine state to RELEASE
}



/**
 * Updates the appropriate parameter of the ADSR enveloppe
 * @param value b/w 0 and 127
 */
void setMidiCCParam(MidiCCParam param, uint8_t value){


	switch (param){

	case VCA_ATTACK:
		vcaAdsr.attackTimeMs = ((value+1)/127.) * MAX_ATTACK_TIME_VCA;
		//stateMachineVca.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/vcaAdsr.attackTimeMs); @deprecated exponential enveloppes
		break;

	case VCA_DECAY:
		vcaAdsr.decayTimeMs = ((value+1)/127.) * MAX_DECAY_TIME_VCA;
		//stateMachineVca.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/vcaAdsr.decayTimeMs); @deprecated exponential enveloppes
		break;

	case VCA_SUSTAIN:
		vcaAdsr.sustainLevel = (value/127.) * MAX_SUSTAIN_LVL_VCA;
		break;

	case VCA_RELEASE:
		vcaAdsr.releaseTimeMs = ((value+1)/127.) * MAX_RELEASE_TIME_VCA;
		//stateMachineVca.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/vcaAdsr.releaseTimeMs); @deprecated exponential enveloppes
		break;

	case VCF_ATTACK:
		vcfAdsr.attackTimeMs = ((value+1)/127.) * MAX_ATTACK_TIME_VCF;
		//stateMachineVcf.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.attackTimeMs); @deprecated exponential enveloppes
		break;

	case VCF_DECAY:
		vcfAdsr.decayTimeMs = ((value+1)/127.) * MAX_DECAY_TIME_VCF;
		//stateMachineVcf.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.decayTimeMs); @deprecated exponential enveloppes
		break;

	case VCF_SUSTAIN:
		vcfAdsr.sustainLevel = (value/127.) * MAX_SUSTAIN_LVL_VCF;
		break;

	case VCF_RELEASE:
		vcfAdsr.releaseTimeMs = ((value+1)/127.) * MAX_RELEASE_TIME_VCF;
		//stateMachineVcf.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.releaseTimeMs); @deprecated exponential enveloppes
		break;

	case VCA_VELOCITY_SENSITIVITY:
		stateMachineVca.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * value/127.;
		break;

	case VCF_VELOCITY_SENSITIVITY:
		stateMachineVcf.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * value/127.;
		break;

	case VCF_RESONANCE:
		dacWrite((int) MAX_RESONANCE * (value/127.), DAC_VCF_RES);
		break;

	case VCF_CUTOFF:
		dacWrite((int) MAX_CUTOFF * (value/127.), DAC_VCF_CUTOFF);
		break;

	case PWM_3340:
		dacWrite((int) (MAX_VCO3340_PWM_DUTY * value/127.), DAC_VCO_3340_PWM_DUTY);
		break;

	case SYNC_3340:
		HAL_GPIO_WritePin(SYNC_3340_GPIO_Port, SYNC_3340_Pin, value==127 ? GPIO_PIN_SET:GPIO_PIN_RESET);
		break;

	case VCF_ORDER :
		if (value == 0){ // 2nd order
			HAL_GPIO_WritePin(VCF_4THORDER_GPIO_Port, VCF_4THORDER_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(VCF_2NDORDER_GPIO_Port, VCF_2NDORDER_Pin, GPIO_PIN_SET);
		}
		else if (value ==1){ // 4th order
			HAL_GPIO_WritePin(VCF_2NDORDER_GPIO_Port, VCF_2NDORDER_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(VCF_4THORDER_GPIO_Port, VCF_4THORDER_Pin, GPIO_PIN_SET);
		}
		break;

	case WAVE_3340 :
		if (value == 0){ // pulse
			HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_SET);
		}
		else if (value == 1){ // triangle
			HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_SET);
		}
		else if (value == 2){ // sawtooth
			HAL_GPIO_WritePin(TRI_3340_GPIO_Port, TRI_3340_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PULSE_3340_GPIO_Port, PULSE_3340_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SAW_3340_GPIO_Port, SAW_3340_Pin, GPIO_PIN_SET);
		}
		break;

	case OCTAVE_3340 :
		vco3340.octave = value;
		break;

	case LEVEL_3340 :
		dacWrite((int) (MAX_VCO3340_LEVEL * value/127.), DAC_V2140D_3340_LVL);
		break;

	case DETUNE_13700 :
		vco13700.detune = value;
		break;


	case LEVEL_13700 :
		// TODO : Voir comment implementer vis-a-vis des mixers
		break;

	case OCTAVE_13700 :
		vco13700.octave = value;
		break;

	case WAVE_13700 :
		//TODO : dacWrite((int) 4095 * (value/127.) * MAX_MIXER, DAC_V2140D_IN2);
		//TODO : dacWrite((int) 4095 * (1 - value/127.) * MAX_MIXER, DAC_V2140D_IN3);
		break;

	case VCF_KBDTRACKING :
		//TODO
		break;

	case VCF_EG :
		//TODO
		break;

	case CALIBRATE :
		runVcoCalibration();
		break;

	case UNUSED_CC:
		break;
	}
}

/**
 * Generally called when a MIDI message of three successive bytes has been received on the SPI bus,
 * @param status a MIDI status byte, e.g., MIDI CC or NOTE ON
 */
void processIncomingMidiMessage(uint8_t statusChannel, uint8_t data1, uint8_t data2){

	int channel = statusChannel & 0x0F;
	int status = statusChannel & 0xF0;

	switch (status){
	case NOTE_ON :
		if (channel == DRUM_CHANNEL){
			// syd 8/9/19 playDrumMachine(data1, data2);
		}
		else {
			midiNote.note = data1;
			midiNote.velocity = data2;
			midiNoteOnHandler();
		}
		break;

	case NOTE_OFF :
		midiNoteOffHandler();
		break;

	case CONTROL_CHANGE:
		setMidiCCParam(midiccCodeToParams[data1], data2);
		break;
	}
}

/**
 * Triggers a GPIO output to play a drum sound
 * @param data1, data2, last two bytes of midi message
 */

void playDrumMachine(uint8_t data1, uint8_t data2){
	switch(data1){
	case BASS_DRUM_NOTE:
		HAL_GPIO_WritePin(DRUM_KICK_GPIO_Port, DRUM_KICK_Pin, GPIO_PIN_RESET);
		drumTriggers.bassdrumCounter = 1;
		break;
	case RIMSHOT_NOTE:
		HAL_GPIO_WritePin(DRUM_RIM_GPIO_Port, DRUM_RIM_Pin, GPIO_PIN_RESET);
		drumTriggers.rimshotCounter = 1;
		break;
	case SNARE_NOTE:
		HAL_GPIO_WritePin(DRUM_SNARE_GPIO_Port, DRUM_SNARE_Pin, GPIO_PIN_SET);
		drumTriggers.snareCounter = 1;
		break;
	case LOWTOM_NOTE:
		HAL_GPIO_WritePin(DRUM_LOWTOM_GPIO_Port, DRUM_LOWTOM_Pin, GPIO_PIN_RESET);
		drumTriggers.lowtomCounter= 1;
		break;
	case HIGHTOM_NOTE:
		HAL_GPIO_WritePin(DRUM_HIGHTOM_GPIO_Port, DRUM_HIGHTOM_Pin, GPIO_PIN_RESET);
		drumTriggers.hightomCounter = 1;
		break;
	}
}

/**
 * called by timer to update trigger signal for drums
 */
void updateDrumMachine(){

	if (drumTriggers.bassdrumCounter >0) {
		drumTriggers.bassdrumCounter --;
		if(drumTriggers.bassdrumCounter ==0) HAL_GPIO_WritePin(DRUM_KICK_GPIO_Port, DRUM_KICK_Pin, GPIO_PIN_SET);
	}
	if (drumTriggers.rimshotCounter >0) {
		drumTriggers.rimshotCounter --;
		if(drumTriggers.rimshotCounter ==0) HAL_GPIO_WritePin(DRUM_RIM_GPIO_Port, DRUM_RIM_Pin, GPIO_PIN_SET);
	}
	if (drumTriggers.snareCounter >0) {
		drumTriggers.snareCounter --;
		if(drumTriggers.snareCounter ==0) HAL_GPIO_WritePin(DRUM_SNARE_GPIO_Port, DRUM_SNARE_Pin, GPIO_PIN_RESET);
	}
	if (drumTriggers.lowtomCounter>0) {
		drumTriggers.lowtomCounter--;
		if(drumTriggers.lowtomCounter==0) HAL_GPIO_WritePin(DRUM_LOWTOM_GPIO_Port, DRUM_LOWTOM_Pin, GPIO_PIN_SET);
	}
	if (drumTriggers.hightomCounter >0) {
		drumTriggers.hightomCounter --;
		if(drumTriggers.hightomCounter ==0) HAL_GPIO_WritePin(DRUM_HIGHTOM_GPIO_Port, DRUM_HIGHTOM_Pin, GPIO_PIN_SET);
	}
}

/* play a demo by manually creating midi messages ; normally gets called every ms from the timer handler */
void playDemo(){

	if (demoCounter == 0) {
		processIncomingMidiMessage(NOTE_ON, demoNotes[demoNoteCounter++], 50); // creates a NOTE ON event
		//playDrumMachine(BASS_DRUM_NOTE, 0);

	}
	else if (demoCounter == 200) {
		processIncomingMidiMessage(NOTE_OFF, 36, 0);
	}
	else if (demoCounter == 400) demoCounter=-1;
	demoCounter++;
	if (demoNoteCounter >= DEMO_NOTES_LEN) demoNoteCounter = 0;


}

// -------------- arxiv ----------------
// move unused functions below so as to keep track of the code in the future if turns out they become useful again

/**
 * called when a new byte arrives on the SPI bus, following the classical
 * three byte MIDI format, namely:
 * - one status byte starting with 1
 * - two data bytes starting with 0
 * So that this function should be called thrice in a row.
 * TODO : (SR) we should add a time out feature so that if a data byte arrives really
 * late  after a status byte, (more than ? ms - this depends on the SPI bus frequency
 * but 1ms might be a good guess) then this is really likely to be the evidence of
 * an error on the SPI bus (coz normally they should come all three in a row)
 */
/*void midiFromSpiMessageHandler(uint8_t byte){

	switch (midi_receiver_state) {

	case WAITING_FOR_BYTE1:

		if ((byte & 0x80) != 0){ // byte starts with 1 -> status byte
			midi_byte1=byte;
			midi_receiver_state = WAITING_FOR_BYTE2;
		}
		// else this means we had more than 2 status bytes and this means there's an error on the SPI bus => wait here
		break;

	case WAITING_FOR_BYTE2:

		if ((byte & 0x80) == 0){ // byte starts with 0 -> data byte
			midi_byte2=byte;
			midi_receiver_state = WAITING_FOR_BYTE3;
		}
		else { // byte starts with a 1, means there's an ISSUE on the SPI bus probably
			// so we consider this is again a status byte
			midi_byte1=byte; // erase old status with new one
			midi_receiver_state = WAITING_FOR_BYTE2; // still waiting for byte 2
		}
		break;

	case WAITING_FOR_BYTE3:

		if ((byte & 0x80) == 0){ // byte starts with zero => data byte
			midi_byte3=byte;
			processIncomingMidiMessage(midi_byte1, midi_byte2, midi_byte3);
			midi_receiver_state = WAITING_FOR_BYTE1; // back to initial state
		}
		else { // ISSUE: byte starts with a 1, means there's an ISSUE on the SPI bus probably
			// so we consider this is again a status byte
			midi_byte1=byte; // erase old status with new one
			midi_receiver_state = WAITING_FOR_BYTE2; // back to waiting for byte 2
		}
		break;
	}
} */


/* exponential enveloppes
void updateVCAEnveloppeStateMachine(){

	double sustain;
	switch (stateMachineVca.machineState){

			case IDLE:
				break;

			case ATTACK:
				stateMachineVca.tmpExp *= stateMachineVca.mulFactorAttack;
				stateMachineVca.amplitude = (1.0 - stateMachineVca.tmpExp) * stateMachineVca.tmpVelocityMulFactor;
				dacVcaWrite(stateMachineVca.amplitude);
				stateMachineVca.t++;
				if (stateMachineVca.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state
					stateMachineVca.t = 0;
					stateMachineVca.tmpExp = 1.0;
					stateMachineVca.machineState = DECAY;
				}
				break;

			case DECAY:

				sustain = stateMachineVca.adsrParam->sustainLevel;
				stateMachineVca.tmpExp *= stateMachineVca.mulFactorDecay;
				stateMachineVca.amplitude = (stateMachineVca.tmpExp * (1 - sustain) + sustain)  * stateMachineVca.tmpVelocityMulFactor;
				dacVcaWrite(stateMachineVca.amplitude);
				stateMachineVca.t++;
				break;

			case RELEASE :

				stateMachineVca.tmpExp *= stateMachineVca.mulFactorRelease;
				dacVcaWrite(stateMachineVca.amplitude * stateMachineVca.tmpExp);
				stateMachineVca.t++;
				if (stateMachineVca.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state = IDLE
					stateMachineVca.t = 0;
					stateMachineVca.machineState = IDLE;
				}
				break;
		}
}
*/

/*
 * void updateVCFEnveloppeStateMachine(){

	double sustain;

	switch (stateMachineVcf.machineState){

			case IDLE:
				break;

			case ATTACK:
				stateMachineVcf.tmpExp *= stateMachineVcf.mulFactorAttack; // = exp(-t/tau)
				stateMachineVcf.cutoffFrequency = globalParams.vcfCutoff + (1.0 - stateMachineVcf.tmpExp) * (stateMachineVcf.tmpMaxLevel - globalParams.vcfCutoff);
				dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency);
				stateMachineVcf.t++;
				if (stateMachineVcf.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state
					stateMachineVcf.t = 0;
					stateMachineVcf.tmpExp = 1.0;
					stateMachineVcf.machineState = DECAY;
				}
				break;

			case DECAY:

				sustain = (stateMachineVcf.adsrParam->sustainLevel) * stateMachineVcf.tmpMaxLevel;
				stateMachineVcf.tmpExp *= stateMachineVcf.mulFactorDecay;
				stateMachineVcf.cutoffFrequency = globalParams.vcfCutoff + (stateMachineVcf.tmpExp * (stateMachineVcf.tmpMaxLevel - sustain - globalParams.vcfCutoff) + sustain);
				dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency);
				stateMachineVcf.t++;
				break;

			case RELEASE :

				stateMachineVcf.tmpExp *= stateMachineVcf.mulFactorRelease;
				dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency * stateMachineVcf.tmpExp);
				stateMachineVcf.t++;
				if (stateMachineVcf.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state
					stateMachineVcf.t = 0;
					stateMachineVcf.machineState = IDLE;
				}
				break;
		}
}*/

