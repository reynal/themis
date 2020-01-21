/*
 * vcf.c
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey

 * ----------------- 3320 vcf ----------------
 * V3320 : 60mV/decade => from 20Hz to 2kHz = 120mV ; we have a 0.1 attenuator between the DAC and the V3320 CV input => 1.2V.
 *         Hence with 2V range at the DAC output, we can sweep frequency over a bit more than 3 decades!
 *         Finally, since there's an exp converter inside the 3320, kbd_tracking reduces to simply translating the input voltage!
 *
 *         the switch can select 2nd or 4th order lowpass filter
 */


/* Includes ------------------------------------------------------------------*/
#include "vcf.h"
#include "stm32f7xx_hal.h"
#include "main.h"
#include "adsr.h"
#include "mcp4822.h"
#include "stdio.h"


/* External variables --------------------------------------------------------*/

extern StateMachineVcf stateMachineVcf;

/* Private variables ---------------------------------------------------------*/

GlobalFilterParams globalFilterParams = {
		.vcfCutoff = MAX_CUTOFF * DEF_MIDICC_CUTOFF / 127.0,
		.vcfResonance = MAX_RESONANCE * DEF_MIDICC_RESONANCE / 127.0
};

/* Private function prototypes -----------------------------------------------*/

void setVcfOrder(uint8_t value){
	value = value % 2;
	if (value == 0){ // 2nd order
		HAL_GPIO_WritePin(VCF_4THORDER_GPIO_Port, VCF_4THORDER_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(VCF_2NDORDER_GPIO_Port, VCF_2NDORDER_Pin, GPIO_PIN_SET);
	}
	else if (value ==1){ // 4th order
		HAL_GPIO_WritePin(VCF_2NDORDER_GPIO_Port, VCF_2NDORDER_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(VCF_4THORDER_GPIO_Port, VCF_4THORDER_Pin, GPIO_PIN_SET);
	}
}

void setVcfCutoffGlobal(uint8_t value){
	globalFilterParams.vcfCutoff = value/127.0;
}


void setVcfResonanceGlobal(uint8_t value){
	globalFilterParams.vcfResonance = value;

}

void updateVcfResonance(){
	dacWrite((int) (MAX_RESONANCE * globalFilterParams.vcfResonance/127.), DAC_VCF_RES);
}


void initVcf(){
	stateMachineVcf.cutoffFrequency = globalFilterParams.vcfCutoff;
	setVcfOrder(0);
}

void updateVcfCutoff(){

	// cutoff frequency varies as opposed to control voltage:
	double cutoff = stateMachineVcf.cutoffFrequency; // +stateMachineVcf.tmpKbdtrackingShiftFactor; // TODO : + dbg_modulation
	int dacLvl = (int)(4095.0 * (1.0-cutoff));
	dacWrite(dacLvl, DAC_VCF_CUTOFF);
}


// ------------------------- hardware test ------------------------

void testVcf(){

	int i=0;
	while(1){
		setVcfCutoffGlobal(i);
		//updateVcfCutoff();
		HAL_Delay(50); // 200ms
		toggleRedLED();
		i ++;
		//printf("%d\n", i++);
		if (i > 127) break;
	}
}

