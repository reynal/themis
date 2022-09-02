/*
 * vco.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_VCO_H_
#define INC_VCO_H_

#include "stm32f4xx_hal.h"

#define DEF_MIDI_NOTE 48 // for init purpose only

#define CENTER_OCTAVE 2
#define NO_DETUNE 64
#define NO_SHIFT 11

// VCO3340 A
#define DEF_MIDICC_VCO3340A_PWM_DUTY 127 // pwm
#define MAX_VCO3340A_PWM_DUTY 150


// VCO3340 B
#define DEF_MIDICC_VCO3340B_PWM_DUTY 127 // pwm
#define MAX_VCO3340B_PWM_DUTY 150


/*
 * struct for VCO parameters
 */
typedef struct {
	uint8_t detune; //-50%-50% of one tone
	uint8_t octave; // can be positive or negative
	uint8_t semitones; // from -11 to +11 semitones
	uint8_t pwm;
} Vco3340AParameters_t;

typedef struct {
	uint8_t detune; //-50%-50% of one tone
	uint8_t octave; // can be positive or negative
	uint8_t semitones; // from -11 to +11 semitones
	uint8_t pwm;
} Vco3340BParameters_t;

/* Private function prototypes -----------------------------------------------*/

//3340A:
void vcoSet3340AOctave(uint8_t midiValue);
void vcoSet3340ASemitones(uint8_t midiValue);
void vcoSet3340ADetune(uint8_t value);
void vcoWrite3340AFreqToDac();
void vcoSet3340APWMDuty(uint8_t midiValue);
void vcoWrite3340APWMDutyToDac();


//3340B:
void vcoSet3340BOctave(uint8_t midiValue);
void vcoSet3340BSemitones(uint8_t midiValue);
void vcoSet3340BDetune(uint8_t value);
void vcoWrite3340BFreqToDac();
void vcoSet3340BPWMDuty(uint8_t midiValue);
void vcoWrite3340BPWMDutyToDac();

// calibration:
void vcoPrepareCalibration();

void vcoInit();
void vcoTestScale();



#endif /* INC_VCO_H_ */
