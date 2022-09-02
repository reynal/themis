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
#include "bh2221.h"
#include "vcf.h"
#include "stm32f4xx_hal.h"
#include "adsr.h"
#include "leds.h"

/* External variables --------------------------------------------------------*/

extern VcfStateMachine_t vcfStateMachine;

/* Variables ---------------------------------------------------------*/

GlobalFilterParams vcfGlobalParams = {
		.vcfCutoff = MAX_CUTOFF * DEF_MIDICC_CUTOFF / 127.0,
		.vcfResonance = MAX_RESONANCE * DEF_MIDICC_RESONANCE / 127.0
};

/* Function prototypes -----------------------------------------------*/

/* User code -----------------------------------------------*/

void vcfSetOrder(uint8_t midiValue){
	midiValue = midiValue % 2;
	if (midiValue == 0){ // 2nd order
		//mcp23017_Set_Vcf_2ndOrder(); // TODO
	}
	else if (midiValue ==1){ // 4th order
		//mcp23017_Set_Vcf_4thOrder(); // TODO
	}
}

void vcfSetGlobalCutoff(uint8_t midiValue){
	vcfGlobalParams.vcfCutoff = midiValue/127.0;
}


void vcfSetGlobalResonance(uint8_t midiValue){
	vcfGlobalParams.vcfResonance = midiValue;

}

void vcfWriteResonanceToDac(){
	bh2221WriteDmaBuffer((int) (MAX_RESONANCE * vcfGlobalParams.vcfResonance/127.), BH2221_VCF_RES);
}


void vcfInit(){
	vcfStateMachine.cutoffFrequency = vcfGlobalParams.vcfCutoff;
	vcfSetOrder(0);
	vcfWriteCutoffToDac();
	vcfWriteResonanceToDac();
}

void vcfWriteCutoffToDac(){

	// cutoff frequency varies as opposed to control voltage:
	double cutoff = vcfStateMachine.cutoffFrequency; // +stateMachineVcf.tmpKbdtrackingShiftFactor; // TODO : + dbg_modulation
	int dacLvl = (int)(4095.0 * (1.0-cutoff));
	//int dacLvl = (int)(4095.0 * (cutoff));
	if (dacLvl<0) dacLvl=0;
	else if (dacLvl>4095) dacLvl=4095;
	bh2221WriteDmaBuffer(dacLvl, BH2221_VCF_CUTOFF);
}


// ------------------------- hardware test ------------------------

void vcfTest(){

	int i=0;
	while(1){
		vcfSetGlobalCutoff(i);
		//updateVcfCutoff();
		HAL_Delay(50); // 200ms
		ledToggle(LED_GREEN);
		i ++;
		//printf("%d\n", i++);
		if (i > 127) break;
	}
}


