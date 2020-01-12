/*
 * midi.c
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey
 */

/* Includes ------------------------------------------------------------------*/

#include "midi.h"
#include "stm32f7xx_hal.h"
#include "misc.h"
#include "dac_board.h"
#include "stdio.h"
#include "adsr.h"
#include "mcp4822.h"
#include "vco.h"
#include "vco_calibration.h"
#include "vcf.h"
#include "drums.h"

/* External variables --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

MidiNote midiNote = { .note = 60, .velocity = 100}; // saves the note inside the last midi message
// midi_receiver_state_t midi_receiver_state; // midi state-machine state
// uint8_t midi_byte1, midi_byte2, midi_byte3; // tmp vars for midi state-machine
Boolean dbg_noteOn = FALSE;

// converts a MIDI CC data1 value to the corresponding enum constant in MidiCCParam
MidiCCParam midiccCodeToParams[128] = {
		UNUSED_CC, 			// 0
		UNUSED_CC, 			// 1
		UNUSED_CC, 			// 2
		UNUSED_CC, 			// 3
		UNUSED_CC, 			// 4
		UNUSED_CC, 			// 5
		UNUSED_CC, 			// 6
		UNUSED_CC, 			// 7 (volume)
		UNUSED_CC, 			// 8
		UNUSED_CC, 			// 9
		UNUSED_CC, 			// 10
		UNUSED_CC, 			// 11
		UNUSED_CC, 			// 12 (effect control 1)
		UNUSED_CC, 			// 13 (effect control 2)
		DETUNE_3340_A,	 	// 14
		WAVE_3340_A, 		// 15
		PWM_3340_A, 		// 16
		LEVEL_3340_A, 		// 17
		DETUNE_3340_B, 		// 18
		LEVEL_TRI_3340_B,	// 19
		LEVEL_PULSE_3340_B, // 20
		LEVEL_SAW_3340_B,	// 21
		PWM_3340_B, 		// 22
		DETUNE_13700, 		// 23
		LEVEL_TRI_13700, 	// 24
		LEVEL_SQU_13700, 	// 25
		UNUSED_CC, 			// 26
		UNUSED_CC, 			// 27
		VCF_CUTOFF, 		// 28 (MSB)
		UNUSED_CC, 			// 29
		VCF_RESONANCE,		// 30
		VCF_KBDTRACKING,	// 31
		VCF_ATTACK,			// 32
		VCF_DECAY,			// 33
		VCF_SUSTAIN,		// 34
		VCF_RELEASE,		// 35
		VCF_EG,				// 36
		VCA_ATTACK,			// 37
		VCA_DECAY,			// 38
		VCA_SUSTAIN,		// 39
		VCA_RELEASE,		// 40
		UNUSED_CC, 			// 41
		UNUSED_CC, 			// 42
		UNUSED_CC, 			// 43
		UNUSED_CC, 			// 44
		UNUSED_CC, 			// 45
		UNUSED_CC, 			// 46
		UNUSED_CC, 			// 47
		UNUSED_CC, 			// 48
		UNUSED_CC, 			// 49
		UNUSED_CC, 			// 50
		UNUSED_CC, 			// 51 (cut off LSB)
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
		SEMITONES_3340_A, 	// 71
		SEMITONES_3340_B, 	// 72
		SEMITONES_13700, 	// 73
		OCTAVE_3340_A, 	// 74
		OCTAVE_3340_B, 	// 75
		OCTAVE_13700,	// 76
		SYNC_3340_A, 	// 77
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


/* Private function prototypes -----------------------------------------------*/

/**
 *  Prepare the envelope state machines following a MIDI NOTE ONE message
 */
void midiNoteOnHandler(){

	if (dbg_noteOn ==TRUE) return; // debounce button

	// printf("Note On\n");
	dbg_noteOn = TRUE;

	switchRedLEDOn(); 	// switch on LED so that we can monitor enveloppe level TODO : pwm !

	// prepare state machines:
	prepareVcaEnvelopeNoteON();
	prepareVcfEnvelopeNoteON();

}

/**
 *  Prepare the envelopes state machines following a MIDI NOTE ONE message
 */
void midiNoteOffHandler(){

	if (dbg_noteOn == FALSE) return; // debounce button

	// printf("Note Off\n");
	dbg_noteOn = FALSE;

	switchRedLEDOff();

	// prepare state machines:
	prepareVcaEnvelopeNoteOFF();
	prepareVcfEnvelopeNoteOFF();

}



/**
 * Updates the appropriate parameter of the ADSR enveloppe
 * @param value b/w 0 and 127
 */
void setMidiCCParam(MidiCCParam param, uint8_t value){


	//printf("setMidiCCParam\n");
	toggleBlueLED();

	switch (param){

		// ------------------------------ VCA ------------------------------
	case VCA_ATTACK:
		// printf("VCA A=%d\n", value);
		setVcaAdsrAttack(value);
		break;

	case VCA_DECAY:
		// printf("VCA D=%d\n", value);
		setVcaAdsrDecay(value);
		break;

	case VCA_SUSTAIN:
		// printf("VCA S=%d\n", value);
		setVcaAdsrSustain(value);
		break;

	case VCA_RELEASE:
		// printf("VCA R=%d\n", value);
		setVcaAdsrRelease(value);
		break;

		// ------------------------------ VCF ------------------------------
	case VCF_ATTACK:
		// printf("VCF Attack=%d\n", value);
		setVcfAdsrAttack(value);
		break;

	case VCF_DECAY:
		// printf("VCF Decay=%d\n", value);
		setVcfAdsrDecay(value);
		break;

	case VCF_SUSTAIN:
		// printf("VCF Sustain=%d\n", value);
		setVcfAdsrSustain(value);
		break;

	case VCF_RELEASE:
		// printf("VCF Release=%d\n", value);
		setVcfAdsrRelease(value);
		break;

	case VCF_KBDTRACKING :
		// printf("VCF KBDTracking=%d\n", value);
		setVcfKbdTracking(value);
		break;

	case VCF_EG :
		// printf("VCF EG=%d\n", value);
		setVcfEgDepth(value);
		break;

		// ------------------------------ velocity ------------------------------
	case VCA_VELOCITY_SENSITIVITY:
		// printf("VCA Velo sens=%d\n", value);
		setVcaVelocitySensitivity(value);
		break;

	case VCF_VELOCITY_SENSITIVITY:
		// printf("VCF Velo sens=%d\n", value);
		setVcfVelocitySensitivity(value);
		break;

		// ------------------------------ filter ------------------------------
	case VCF_CUTOFF:
		// printf("VCF Cutoff=%d\n", value);
		setVcfCutoffGlobal(value);
		break;

	case VCF_RESONANCE:
		// printf("VCF Res=%d\n", value);
		setVcfResonanceGlobal(value);
		break;

	case VCF_ORDER :
		// printf("VCF Order=%d\n", value);
		setVcfOrder(value);
		break;


		// ------------------------------ VCO 13700 ------------------------------

	case OCTAVE_13700 :
		// printf("VCO13700 Octave=%d\n", value);
		setVco13700Octave(value);
		break;

	case SEMITONES_13700 :
		setVco13700Semitones(value);
		break;

	case DETUNE_13700 :
		// printf("VCO13700 Detune=%d\n", value);
		setVco13700Detune(value);
		break;

	case LEVEL_TRI_13700 :
		// printf("VCO13700 Tri=%d\n", value);
		setVco13700TriLevel(value);
		break;

	case LEVEL_SQU_13700 :
		// printf("VCO13700 Squ=%d\n", value);
		setVco13700SquareLevel(value);
		break;

		// ------------------------------ VCO 3340A------------------------------

	case OCTAVE_3340_A:
		// printf("VCO3340A Octave=%d\n", value);
		setVco3340AOctave(value);
		break;

	case SEMITONES_3340_A :
		setVco3340ASemitones(value);
		break;

	case DETUNE_3340_A:
		// printf("VCO3340A Detune=%d\n", value);
		setVco3340ADetune(value);
		break;

	case LEVEL_3340_A:
		// printf("VCO3340A Level=%d\n", value);
		setVco3340ALevel(value);
		break;

	case WAVE_3340_A:
		// printf("VCO3340A Wave=%d\n", value);
		setVco3340AWaveType(value);
		break;

	case PWM_3340_A:
		// printf("VCO3340A PWM=%d\n", value);
		setVco3340APWMDuty(value);
		break;

	case SYNC_3340_A:
		// printf("VCO3340A Sync=%d\n", value);
		setVco3340ASync(value);
		break;


		// ------------------------------ VCO3340B ------------------------------

	case OCTAVE_3340_B:
		// printf("VCO3340B Octave=%d\n", value);
		setVco3340BOctave(value);
		break;

	case SEMITONES_3340_B :
		setVco3340BSemitones(value);
		break;

	case DETUNE_3340_B:
		// printf("VCO3340B Detune=%d\n", value);
		setVco3340BDetune(value);
		break;

	case PWM_3340_B:
		// printf("VCO3340B PWM=%d\n", value);
		setVco3340BPWMDuty(value);
		break;

	case LEVEL_TRI_3340_B:
		// printf("VCO3340B Tri level=%d\n", value);
		setVco3340BTriLevel(value);
		break;

	case LEVEL_SAW_3340_B:
		// printf("VCO3340B Saw level=%d\n", value);
		setVco3340BSawLevel(value);
		break;

	case LEVEL_PULSE_3340_B:
		// printf("VCO3340B Pulse level=%d\n", value);
		setVco3340BPulseLevel(value);
		break;

		// ------------------------------ MISC ------------------------------

	case CALIBRATE :
		// printf("Calibrate=%d\n", value);
		//runVcoCalibration();
		break;

	case UNUSED_CC:
		// printf("%d UNUSED!\n", param);
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
			midiNote.note = data1; // TODO SR Dec 3rd 2019: maybe move these two lines into miniNoteOnHandler() for coherence?
			midiNote.velocity = data2; // but this implies changing the prototype of midiNoteOnHandler()
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

