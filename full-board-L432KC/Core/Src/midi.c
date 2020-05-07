/*
 * midi.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

/* Includes ------------------------------------------------------------------*/

#include "midi.h"
#include "stm32l4xx_hal.h"
#include "stdio.h"
#include "adsr.h"
#include "vco.h"
#include "vcf.h"
#include "drums.h"
#include "leds.h"

/* External variables --------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void process_Message();
static void noteOn_Handler(uint8_t note, uint8_t vel);
static void noteOff_Handler(uint8_t note);

/* Private variables ---------------------------------------------------------*/

MidiNote midi_Note = { .note = 60, .velocity = 100}; // saves the note inside the last midi message
Midi_Receiver_State midi_Receiver_State; // midi state-machine state
Midi_Message midi_Message; // tmp var for state-machine

Boolean dbg_noteOn = FALSE;

// converts a MIDI CC data1 value to the corresponding enum constant in MidiCCParam TODO replace by a table of setXXXParam(value) functions
// and call this function directly from process_Message():
MidiCCParam midiCC_To_Params[128] = {
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




/* User code -----------------------------------------------*/

/**
 *  Prepare the envelope state machines following a MIDI NOTE ONE message
 */
static void noteOn_Handler(uint8_t note, uint8_t vel){

	printf("Note on: %d %d\n", note, vel);

	if (dbg_noteOn ==TRUE) return; // debounce button

	midi_Note.note = note; // TODO SR Dec 3rd 2019: maybe move these two lines into miniNoteOnHandler() for coherence?
	midi_Note.velocity = vel; // but this implies changing the prototype of midiNoteOnHandler()


	// printf("Note On\n");
	dbg_noteOn = TRUE;

	switchGreenLEDOn(); 	// switch on LED so that we can monitor enveloppe level TODO : pwm !

	// prepare state machines:
	prepare_Vca_Envelope_NoteON();
	prepare_Vcf_Envelope_NoteON();

}

/**
 *  Prepare the envelopes state machines following a MIDI NOTE ONE message
 */
static void noteOff_Handler(uint8_t note){

	printf("Note off: %d\n", note);

	if (dbg_noteOn == FALSE) return; // debounce button

	// TODO check if note == midiNote.note: if not, we're releasing a note that is no longer being played => do nothing

	// printf("Note Off\n");
	dbg_noteOn = FALSE;

	switchGreenLEDOff();

	// prepare state machines:
	prepare_Vca_Envelope_NoteOFF();
	prepare_Vcf_Envelope_NoteOFF();

}



/**
 * Updates the appropriate parameter of the ADSR enveloppe
 * @param value b/w 0 and 127
 */
void midi_Set_Param_From_CC(MidiCCParam param, uint8_t value){

	printf("MIDI CC: %d %d\n", param, value);


	//printf("setMidiCCParam\n");
	toggleRedLED();

	switch (param){

		// ------------------------------ VCA ------------------------------
	case VCA_ATTACK:
		// printf("VCA A=%d\n", value);
		set_Vca_AdsrAttack(value);
		break;

	case VCA_DECAY:
		// printf("VCA D=%d\n", value);
		set_Vca_AdsrDecay(value);
		break;

	case VCA_SUSTAIN:
		// printf("VCA S=%d\n", value);
		set_Vca_AdsrSustain(value);
		break;

	case VCA_RELEASE:
		// printf("VCA R=%d\n", value);
		set_Vca_AdsrRelease(value);
		break;

		// ------------------------------ VCF ------------------------------
	case VCF_ATTACK:
		// printf("VCF Attack=%d\n", value);
		set_Vcf_AdsrAttack(value);
		break;

	case VCF_DECAY:
		// printf("VCF Decay=%d\n", value);
		set_Vcf_AdsrDecay(value);
		break;

	case VCF_SUSTAIN:
		// printf("VCF Sustain=%d\n", value);
		set_Vcf_AdsrSustain(value);
		break;

	case VCF_RELEASE:
		// printf("VCF Release=%d\n", value);
		set_Vcf_AdsrRelease(value);
		break;

	case VCF_KBDTRACKING :
		// printf("VCF KBDTracking=%d\n", value);
		set_Vcf_KbdTracking(value);
		break;

	case VCF_EG :
		// printf("VCF EG=%d\n", value);
		set_Vcf_EgDepth(value);
		break;

		// ------------------------------ velocity ------------------------------
	case VCA_VELOCITY_SENSITIVITY:
		// printf("VCA Velo sens=%d\n", value);
		set_Vca_VelocitySensitivity(value);
		break;

	case VCF_VELOCITY_SENSITIVITY:
		// printf("VCF Velo sens=%d\n", value);
		set_Vcf_VelocitySensitivity(value);
		break;

		// ------------------------------ filter ------------------------------
	case VCF_CUTOFF:
		// printf("VCF Cutoff=%d\n", value);
		set_Vcf_CutoffGlobal(value);
		break;

	case VCF_RESONANCE:
		// printf("VCF Res=%d\n", value);
		set_Vcf_ResonanceGlobal(value);
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
 * Generally called when a MIDI message of three successive bytes has been received on the bus,
 * Parameters are taken from the midi_Message shared variable.
 */
static void process_Message(){

	int channel = midi_Message.status_channel & 0x0F;
	int status = midi_Message.status_channel & 0xF0;

	switch (status){
	case NOTE_ON :
		if (channel == DRUM_CHANNEL){
			// syd 8/9/19 playDrumMachine(data1, data2);
		}
		else {
			noteOn_Handler(midi_Message.data1, midi_Message.data2);
		}
		break;

	case NOTE_OFF :
		noteOff_Handler(midi_Message.data1);
		break;

	case CONTROL_CHANGE:
		midi_Set_Param_From_CC(midiCC_To_Params[midi_Message.data1], midi_Message.data2);
		break;
	}
}

/**
 * Generally called when a MIDI message of three successive bytes has been received on the bus,
 * @param statusChannel a MIDI status byte, e.g., MIDI CC or NOTE ON
 */
void midi_Process_Incoming_Message(uint8_t statusChannel, uint8_t data1, uint8_t data2){

	midi_Message.status_channel = statusChannel;
	midi_Message.data1 = data1;
	midi_Message.data2 = data2;
	process_Message();

}


/**
 * called when a new byte arrives on the MIDI bus (either SPI or UART)
 * following the classical three byte MIDI format, namely:
 * - one status byte starting with 1
 * - two data bytes starting with 0
 *
 * This function modifies the global variable "midi_Message".
 */
void midi_Process_Byte(uint8_t byte){

	switch (midi_Receiver_State) {

	case WAITING_FOR_BYTE1:

		if ((byte & 0x80) != 0){ // byte starts with 1 -> status byte
			midi_Message.status_channel = byte;
			midi_Receiver_State = WAITING_FOR_BYTE2;
		}
		// else this means we had more than 2 status bytes and this means there's an error on the MIDI bus => wait here
		break;

	case WAITING_FOR_BYTE2:

		if ((byte & 0x80) == 0){ // byte starts with 0 -> data byte
			midi_Message.data1 = byte;
			midi_Receiver_State = WAITING_FOR_BYTE3;
		}
		else { // byte starts with a 1, means there's an ISSUE on the bus probably
			// so we consider this is again a status byte
			midi_Message.status_channel = byte; // erase old status with new one
			midi_Receiver_State = WAITING_FOR_BYTE2; // still waiting for byte 2
		}
		break;

	case WAITING_FOR_BYTE3:

		if ((byte & 0x80) == 0){ // byte starts with zero => data byte
			midi_Message.data2 = byte;
			process_Message();
			midi_Receiver_State = WAITING_FOR_BYTE1; // back to initial state
		}
		else { // ISSUE: byte starts with a 1, means there's an ISSUE on the MIDI bus probably
			// so we consider this is again a status byte
			midi_Message.data1 = byte; // erase old status with new one
			midi_Receiver_State = WAITING_FOR_BYTE2; // back to waiting for byte 2
		}
		break;
	}
}


