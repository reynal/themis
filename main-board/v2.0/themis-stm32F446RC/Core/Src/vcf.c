/*
 * vcf.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 *
 * ----------------- 3320 vcf ----------------
 * V3320 : 60mV/decade => from 20Hz to 2kHz = 120mV ; we have a 0.1 attenuator between the DAC and the V3320 CV input => 1.2V.
 *         Hence with 2V range at the DAC output, we can sweep frequency over a bit more than 3 decades!
 *         Finally, since there's an exp converter inside the 3320, kbd_tracking reduces to simply translating the input voltage!
 *
 *         the switch can select 2nd or 4th order lowpass filter
  *
 */

/* Includes ------------------------------------------------------------------*/
#include "ad5644.h"
#include "vcf.h"
#include "stm32f4xx_hal.h"
#include "adsr.h"
//#include "stdio.h"
#include "leds.h"
#include "mcp23017.h"

/* External variables --------------------------------------------------------*/

extern State_Machine_Vcf stateMachineVcf;

/* Variables ---------------------------------------------------------*/

GlobalFilterParams globalFilterParams = {
		.vcfCutoff = MAX_CUTOFF * DEF_MIDICC_CUTOFF / 127.0,
		.vcfResonance = MAX_RESONANCE * DEF_MIDICC_RESONANCE / 127.0
};

/* Function prototypes -----------------------------------------------*/

/* User code -----------------------------------------------*/

void setVcfOrder(uint8_t midiValue){
	midiValue = midiValue % 2;
	if (midiValue == 0){ // 2nd order
		mcp23017_Set_Vcf_2ndOrder();
	}
	else if (midiValue ==1){ // 4th order
		mcp23017_Set_Vcf_4thOrder();
	}
}

void set_Vcf_CutoffGlobal(uint8_t midiValue){
	globalFilterParams.vcfCutoff = midiValue/127.0;
}


void set_Vcf_ResonanceGlobal(uint8_t midiValue){
	globalFilterParams.vcfResonance = midiValue;

}

void updateVcfResonance(){
	dacWrite((int) (MAX_RESONANCE * globalFilterParams.vcfResonance/127.), DAC_VCF_RES);
}


void init_Vcf(){
	stateMachineVcf.cutoffFrequency = globalFilterParams.vcfCutoff;
	setVcfOrder(0);
	updateVcfCutoff();
	updateVcfResonance();
}

void updateVcfCutoff(){

	// cutoff frequency varies as opposed to control voltage:
	double cutoff = stateMachineVcf.cutoffFrequency; // +stateMachineVcf.tmpKbdtrackingShiftFactor; // TODO : + dbg_modulation
	int dacLvl = (int)(4095.0 * (1.0-cutoff));
	//int dacLvl = (int)(4095.0 * (cutoff));
	if (dacLvl<0) dacLvl=0;
	else if (dacLvl>4095) dacLvl=4095;
	dacWrite(dacLvl, DAC_VCF_CUTOFF);
}


// ------------------------- hardware test ------------------------

void testVcf(){

	int i=0;
	while(1){
		set_Vcf_CutoffGlobal(i);
		//updateVcfCutoff();
		HAL_Delay(50); // 200ms
		toggleGreenLED();
		i ++;
		//printf("%d\n", i++);
		if (i > 127) break;
	}
}


