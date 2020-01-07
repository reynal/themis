/*
 * midi.h
 *
 * Collection of enums and struct dedicated to incoming MIDI messages
 *
 *  Created on: Oct 1, 2018
 *      Author: S. Reynal
 */

#ifndef MIDI_H_
#define MIDI_H_

#include "stm32f7xx_hal.h"

#define MAX_MIDI_VALUE 127.0

#define MIDICC_VALUE_OFF 0
#define MIDICC_VALUE_ON 1

/**
 *  a struct for incoming midi messages of type NoteOn/Off
 */
typedef struct {
  int note; // 0-127
  int velocity; // 0-127
} MidiNote;




/* tunable MIDI CC parameters ; parameters are numbered from 0 in the order they are given in the enum */
typedef enum {
	UNUSED_CC,

	OCTAVE_3340_A, // 0 1 2 3
	DETUNE_3340_A,// 0-127, 0% at 64
	WAVE_3340_A,  // 0 1 2
	PWM_3340_A,// 0-127
	LEVEL_3340_A, // 0-127 mixer 1
	SYNC_3340_A, // 0 ou 127

	OCTAVE_3340_B, // 0 1 2 3
	DETUNE_3340_B,// 0-127, 0% at 64
	PWM_3340_B,// 0-127
	LEVEL_TRI_3340_B, // 0-127 mixer 1
	LEVEL_SAW_3340_B, // 0-127 mixer 1
	LEVEL_PULSE_3340_B, // 0-127 mixer 1

	OCTAVE_13700, // 0 1 2 3
	DETUNE_13700, // 0-127
	LEVEL_TRI_13700, // 0-127 entre sq et tri (mixer 2 mixer 3)
	LEVEL_SQU_13700, // 0-127 mixer 1

	VCF_CUTOFF,
	VCF_RESONANCE,
	VCF_ORDER,
	VCF_KBDTRACKING,
	VCF_EG,
	VCF_VELOCITY_SENSITIVITY,
	VCF_ATTACK,
	VCF_DECAY,
	VCF_SUSTAIN,
	VCF_RELEASE,

	VCA_VELOCITY_SENSITIVITY,
	VCA_ATTACK,
	VCA_DECAY,
	VCA_SUSTAIN,
	VCA_RELEASE,

	CALIBRATE // send a non zero data to trigger a recalibration

} MidiCCParam;


/*
 * An enum for the state machine that processes MIDI messages three by three
 */
typedef enum {
		WAITING_FOR_BYTE1, // waiting for byte #1
		WAITING_FOR_BYTE2,  // waiting for byte #2
		WAITING_FOR_BYTE3// waiting for byte #3
} midi_receiver_state_t;

/*
 * A collection of constants for MIDI status bytes
 */
#define NOTE_ON 		0x90
#define NOTE_OFF 		0x80
#define CONTROL_CHANGE 	0xB0
#define PITCH_BEND		0xE0

/* Private function prototypes -----------------------------------------------*/

void midiNoteOnHandler();
void midiNoteOffHandler();
void processIncomingMidiMessage(uint8_t status, uint8_t data1, uint8_t data2);
void setMidiCCParam(MidiCCParam param, uint8_t value);


#endif /* MIDI_H_ */
