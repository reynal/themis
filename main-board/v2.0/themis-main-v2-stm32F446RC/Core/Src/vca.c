/*
 * vca.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#include "ad5644.h"
#include "adsr.h"

/* External variables --------------------------------------------------------*/

extern VcaStateMachine_t vcaStateMachine;


/* async update of VCA amplitude from vcaStateMachine.amplitude  */
void vcaWriteAmplitudeToDac(){

	double amplitude = vcaStateMachine.amplitude ; // TODO : + dbg_modulation
	int dacLvl = (int)(4095.0 * amplitude);
	if (dacLvl<0) dacLvl=0;
	else if (dacLvl>4095) dacLvl=4095;
	ad5644WriteDmaBuffer(dacLvl, AD5644_VCA);
}

/* init VCA parameters */
void vcaInit(){

	vcaStateMachine.amplitude = 0;
	vcaWriteAmplitudeToDac();

}