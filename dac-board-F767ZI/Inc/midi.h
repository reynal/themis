/*
 * midi.h
 *
 *  Created on: Oct 1, 2018
 *      Author: S. Reynal
 */

#ifndef MIDI_H_
#define MIDI_H_


// la note actuellement jouée, entre 0 et 127 (norme MIDI) -- ou de la dernière note jouée si elle est "off", et sa vélocité
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
