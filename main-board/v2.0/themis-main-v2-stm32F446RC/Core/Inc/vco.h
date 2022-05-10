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
#define DEF_MIDICC_VCO3340A_PWM_DUTY 127
#define MAX_VCO3340A_PWM_DUTY 2450

#define DEF_MIDICC_VCO3340A_LEVEL 127
#define MAX_VCO3340A_LEVEL 4095

#define DEF_MIDICC_VCO3340A_WAVE 0 // saw


// VCO3340 B
#define DEF_MIDICC_VCO3340B_PWM_DUTY 127 // pwm
#define MAX_VCO3340B_PWM_DUTY 2450

#define DEF_MIDICC_VCO3340B_PULSE_LEVEL 127 // pulse
#define MAX_VCO3340B_PULSE_LEVEL 4095

#define DEF_MIDICC_VCO3340B_SAW_LEVEL 0 // saw
#define MAX_VCO3340B_SAW_LEVEL 4095

#define DEF_MIDICC_VCO3340B_TRI_LEVEL 0 // tri
#define MAX_VCO3340B_TRI_LEVEL 4095

// VCO13700
#define DEF_MIDICC_VCO13700_LEVEL 100

#define DEF_MIDICC_VCO13700_SQU_LEVEL 0 // square
#define MAX_VCO13700_SQU_LEVEL 4095

#define DEF_MIDICC_VCO13700_TRI_LEVEL 127 // tri
#define MAX_VCO13700_TRI_LEVEL 4095

#define DEF_MIDICC_VCO13700_SUBBASS_LEVEL 0 // subbass
#define MAX_VCO13700_SUBBASS_LEVEL 4095

/*
 * struct for VCO parameters
 */
typedef struct {
	uint8_t detune; //-50%-50% of one tone
	uint8_t octave; // can be positive or negative
	uint8_t semitones; // from -11 to +11 semitones
	uint8_t level;
	uint8_t pwm;
} Vco3340AParameters;

typedef struct {
	uint8_t detune; //-50%-50% of one tone
	uint8_t octave; // can be positive or negative
	uint8_t semitones; // from -11 to +11 semitones
	uint8_t tri_level;
	uint8_t saw_level;
	uint8_t pulse_level;
	uint8_t pwm;
} Vco3340BParameters;

typedef struct {
	uint8_t detune; //-50%-50% of one tone
	uint8_t octave; // can be positive or negative
	uint8_t semitones; // from -11 to +11 semitones
	uint8_t tri_level;
	uint8_t squ_level;
	uint8_t subbass_level;
} Vco13700Parameters;

/* Private function prototypes -----------------------------------------------*/

// 13700:
void setVco13700Octave(uint8_t value);
void setVco13700Semitones(uint8_t value);
void setVco13700Detune(uint8_t value);
void updateVco13700Freq();
void setVco13700SquareLevel(uint8_t midiValue);
void updateVco13700SquareLevel();
void setVco13700TriLevel(uint8_t midiValue);
void updateVco13700TriLevel();
void setVco13700SubbassLevel(uint8_t midiValue);
void updateVco13700SubbassLevel();


// 3340A:
void setVco3340AOctave(uint8_t midiValue);
void setVco3340ASemitones(uint8_t midiValue);
void setVco3340ADetune(uint8_t value);
void updateVco3340AFreq();
void setVco3340APWMDuty(uint8_t midiValue);
void updateVco3340APWMDuty();
void setVco3340AWaveType(uint8_t midiValue);
void setVco3340ASync(uint8_t midiValue);
void setVco3340ALevel(uint8_t midiValue);
void updateVco3340ALevel();

//3340B:
void setVco3340BOctave(uint8_t midiValue);
void setVco3340BSemitones(uint8_t midiValue);
void setVco3340BDetune(uint8_t value);
void updateVco3340BFreq();
void setVco3340BPWMDuty(uint8_t midiValue);
void updateVco3340BPWMDuty();
void setVco3340BPulseLevel(uint8_t midiValue);
void updateVco3340BPulseLevel();
void setVco3340BSawLevel(uint8_t midiValue);
void updateVco3340BSawLevel();
void setVco3340BTriLevel(uint8_t midiValue);
void updateVco3340BTriLevel();

// calibration:
void vco_Prepare_For_Calibration();

void init_Vco();
void testScale();



#endif /* INC_VCO_H_ */
