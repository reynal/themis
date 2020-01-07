/*
 * vco.h
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey
 */

#ifndef VCO_H_
#define VCO_H_

#include "stm32f7xx_hal.h"

#define DEF_MIDI_NOTE 48 // for init purpose only

// VCO3340 A
#define DEF_MIDICC_VCO3340A_PWM_DUTY 127
#define MAX_VCO3340A_PWM_DUTY 2450

#define DEF_MIDICC_VCO3340A_LEVEL 127
#define MAX_VCO3340A_LEVEL 4095

#define DEF_MIDICC_VCO3340A_WAVE 0 // saw

#define CENTER_OCTAVE 2
#define NO_DETUNE 64

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
#define DEF_MIDICC_VCO13700_WAVE 64

/*
 * struct for VCO parameters
 */
typedef struct {
	int detune; //-50%-50% of one tone
	int octave; // can be positive or negative
	uint8_t level;
	uint8_t pwm;
} Vco3340AParameters;

typedef struct {
	int detune; //-50%-50% of one tone
	int octave; // can be positive or negative
	uint8_t tri_level;
	uint8_t saw_level;
	uint8_t pulse_level;
	uint8_t pwm;
} Vco3340BParameters;

typedef struct {
	int detune; //-50%-50% of one tone
	int octave; // can be positive or negative
} Vco13700Parameters;

/* Private function prototypes -----------------------------------------------*/

void setVco3340AWaveType(uint8_t v);
void setVco3340ASync(uint8_t value);
void setVco3340AOctave(uint8_t value);
void setVco3340ADetune(uint8_t value);
void setVco3340APWMDuty(uint8_t midiValue);
void setVco3340ALevel(uint8_t midiValue);

void updateVco3340ALevel();
void updateVco3340APWMDuty();

void setVco3340BOctave(uint8_t value);
void setVco3340BDetune(uint8_t value);
void setVco13700Octave(uint8_t value);
void setVco13700Detune(uint8_t value);
void setVco3340BPWMDuty(uint8_t midiValue);
void setVco3340BPulseLevel(uint8_t midiValue);
void setVco3340BTriLevel(uint8_t midiValue);
void setVco3340BSawLevel(uint8_t midiValue);

void updateVco3340BPulseLevel();
void updateVco3340BSawLevel();
void updateVco3340BTriLevel();
void updateVco3340BPWMDuty();

void updateVco13700Freq(); // outputs A4 @ 440Hz
void updateVco3340AFreq();
void updateVco3340BFreq();

void prepareVCOForCalibration();

void initVco();
void testScale();


#endif /* VCO_H_ */
