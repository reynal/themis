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


/* async update of VCA amplitude from stateMachineVca.amplitude  */
void updateVca(){

	double amplitude = stateMachineVca.amplitude ; // TODO : + dbg_modulation
	int dacLvl = (int)(4095.0 * amplitude);
	if (dacLvl<0) dacLvl=0;
	else if (dacLvl>4095) dacLvl=4095;
	dacWrite(dacLvl, DAC_VCA);
}

void init_Vca(){

	stateMachineVca.amplitude = 0;
	updateVca();

}
