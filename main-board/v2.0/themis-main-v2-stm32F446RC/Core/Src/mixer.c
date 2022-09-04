/*
 * mixer.c
 *
 *  Created on: Sep 2, 2022
 *      Author: sydreynal
 */

#include "mixer.h"
#include "bh2221.h"

static MixerParameters_t mixerParameters = {
		.vco3340A_tri_level = DEF_MIDICC_VCO3340A_TRI_LEVEL,
		.vco3340A_saw_level = DEF_MIDICC_VCO3340A_SAW_LEVEL,
		.vco3340A_pulse_level = DEF_MIDICC_VCO3340A_PULSE_LEVEL,
		.vco3340B_tri_level = DEF_MIDICC_VCO3340B_TRI_LEVEL,
		.vco3340B_saw_level = DEF_MIDICC_VCO3340B_SAW_LEVEL,
		.vco3340B_pulse_level = DEF_MIDICC_VCO3340B_PULSE_LEVEL,
		.subbass_level = 0
};

void mixerInit(){

	mixerSetVco3340APulseLevel(DEF_MIDICC_VCO3340A_PULSE_LEVEL); mixerWriteVco3340APulseLevelToDac();
	mixerSetVco3340ASawLevel(DEF_MIDICC_VCO3340A_SAW_LEVEL); mixerWriteVco3340ASawLevelToDac();
	mixerSetVco3340ATriLevel(DEF_MIDICC_VCO3340A_TRI_LEVEL); mixerWriteVco3340ATriLevelToDac();

	mixerSetVco3340BPulseLevel(DEF_MIDICC_VCO3340B_PULSE_LEVEL); mixerWriteVco3340BPulseLevelToDac();
	mixerSetVco3340BSawLevel(DEF_MIDICC_VCO3340B_SAW_LEVEL); mixerWriteVco3340BSawLevelToDac();
	mixerSetVco3340BTriLevel(DEF_MIDICC_VCO3340B_TRI_LEVEL); mixerWriteVco3340BTriLevelToDac();

}

void mixerSetVco3340APulseLevel(uint8_t midiValue){
	mixerParameters.vco3340A_pulse_level = midiValue;
}

void mixerWriteVco3340APulseLevelToDac(){
	bh2221WriteAsync((int)(MAX_VCO3340A_PULSE_LEVEL * (127.0-mixerParameters.vco3340A_pulse_level)/127.0), BH2221_V2140D_3340A_PULSE_LVL);
}


void mixerSetVco3340ASawLevel(uint8_t midiValue){
	mixerParameters.vco3340A_saw_level= midiValue;
}

void mixerWriteVco3340ASawLevelToDac(){
	bh2221WriteAsync((int)(MAX_VCO3340A_SAW_LEVEL * (127.0-mixerParameters.vco3340A_saw_level) /127.0), BH2221_V2140D_3340A_SAW_LVL);
}

void mixerSetVco3340ATriLevel(uint8_t midiValue){
	mixerParameters.vco3340A_tri_level= midiValue;
}

void mixerWriteVco3340ATriLevelToDac(){
	bh2221WriteAsync((int)(MAX_VCO3340A_TRI_LEVEL * (127.0-mixerParameters.vco3340A_tri_level) /127.0), BH2221_V2140D_3340A_TRI_LVL);
}

void mixerSetVco3340BPulseLevel(uint8_t midiValue){
	mixerParameters.vco3340B_pulse_level= midiValue;
}

void mixerWriteVco3340BPulseLevelToDac(){
	bh2221WriteAsync((int)(MAX_VCO3340B_PULSE_LEVEL * (127.0-mixerParameters.vco3340B_pulse_level)/127.0), BH2221_V2140D_3340B_PULSE_LVL);
}


void mixerSetVco3340BSawLevel(uint8_t midiValue){
	mixerParameters.vco3340B_saw_level= midiValue;
}

void mixerWriteVco3340BSawLevelToDac(){
	bh2221WriteAsync((int)(MAX_VCO3340B_SAW_LEVEL * (127.0-mixerParameters.vco3340B_saw_level) /127.0), BH2221_V2140D_3340B_SAW_LVL);
}

void mixerSetVco3340BTriLevel(uint8_t midiValue){
	mixerParameters.vco3340B_tri_level = midiValue;
}

void mixerWriteVco3340BTriLevelToDac(){
	bh2221WriteAsync((int)(MAX_VCO3340B_TRI_LEVEL * (127.0-mixerParameters.vco3340B_tri_level) /127.0), BH2221_V2140D_3340B_TRI_LVL);
}
