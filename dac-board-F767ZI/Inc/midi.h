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



#endif /* MIDI_H_ */
