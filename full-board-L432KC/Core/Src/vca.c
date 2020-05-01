/*
 * vca.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#include "ad5391.h"
#include "adsr.h"

/* External variables --------------------------------------------------------*/

extern State_Machine_Vca stateMachineVca;


/**
 *  write the given amplitude to the VCA control voltage through the appropriate DAC
 *  @param amplitude must be b/w 0 and 1
 */
void dacVcaWrite_Blocking(double amplitude){

	// TODO L4 amplitude = 1.0 - amplitude; // attenuation by the V2140D VCA is proportional to Control Voltage
	int i = (int)(amplitude * 4095);
	if (i<0) i=0;
	else if (i>4095) i=4095;
	dacWrite_Blocking(i, DAC_VCA);
}

/* async update of VCA amplitude from stateMachineVca.amplitude  */
void updateVca(){

	double amplitude = stateMachineVca.amplitude ; // TODO : + dbg_modulation
	// TODO L4 int dacLvl = (int)(4095.0 * (1.0-amplitude));
	int dacLvl = (int)(4095.0 * amplitude);
	if (dacLvl<0) dacLvl=0;
	else if (dacLvl>4095) dacLvl=4095;
	dacWrite(dacLvl, DAC_VCA);
}

