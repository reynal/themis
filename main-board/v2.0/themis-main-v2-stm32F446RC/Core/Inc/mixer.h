/*
 * mixer.h
 *
 *  Created on: Sep 2, 2022
 *      Author: sydreynal
 */

#ifndef INC_MIXER_H_
#define INC_MIXER_H_

#include "stm32f4xx_hal.h"

#define DEF_MIDICC_VCO3340A_PULSE_LEVEL 127 // pulse
#define MAX_VCO3340A_PULSE_LEVEL 255

#define DEF_MIDICC_VCO3340A_SAW_LEVEL 0 // saw
#define MAX_VCO3340A_SAW_LEVEL 255

#define DEF_MIDICC_VCO3340A_TRI_LEVEL 0 // tri
#define MAX_VCO3340A_TRI_LEVEL 255

#define DEF_MIDICC_VCO3340B_PULSE_LEVEL 127 // pulse
#define MAX_VCO3340B_PULSE_LEVEL 255

#define DEF_MIDICC_VCO3340B_SAW_LEVEL 0 // saw
#define MAX_VCO3340B_SAW_LEVEL 255

#define DEF_MIDICC_VCO3340B_TRI_LEVEL 0 // tri
#define MAX_VCO3340B_TRI_LEVEL 255

/*
 * struct for VCO parameters
 */
typedef struct {
	uint8_t vco3340A_tri_level;
	uint8_t vco3340A_saw_level;
	uint8_t vco3340A_pulse_level;
	uint8_t vco3340B_tri_level;
	uint8_t vco3340B_saw_level;
	uint8_t vco3340B_pulse_level;
	uint8_t subbass_level;
} MixerParameters_t;

void mixerInit();

void mixerSetVco3340APulseLevel(uint8_t midiValue);
void mixerWriteVco3340APulseLevelToDac();

void mixerSetVco3340ASawLevel(uint8_t midiValue);
void mixerWriteVco3340ASawLevelToDac();

void mixerSetVco3340ATriLevel(uint8_t midiValue);
void mixerWriteVco3340ATriLevelToDac();

void mixerSetVco3340BPulseLevel(uint8_t midiValue);
void mixerWriteVco3340BPulseLevelToDac();

void mixerSetVco3340BSawLevel(uint8_t midiValue);
void mixerWriteVco3340BSawLevelToDac();

void mixerSetVco3340BTriLevel(uint8_t midiValue);
void mixerWriteVco3340BTriLevelToDac();


#endif /* INC_MIXER_H_ */
