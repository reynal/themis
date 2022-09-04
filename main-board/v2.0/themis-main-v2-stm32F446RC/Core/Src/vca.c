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


/* async update of VCA amplitude from vcaStateMachine.amplitude ; CV is written to AD5644 on 14 bits  */
void vcaWriteAmplitudeToDac(){

	double amplitude = vcaStateMachine.amplitude ; // TODO : + dbg_modulation
	int dacLvl = (int)(AD5644_MAX_VAL * amplitude);
	if (dacLvl<0) dacLvl=0;
	else if (dacLvl > AD5644_MAX_VAL) dacLvl = AD5644_MAX_VAL;
	ad5644WriteAsync(dacLvl, AD5644_VCA);
}

/* init VCA parameters */
void vcaInit(){

	vcaStateMachine.amplitude = 0;
	vcaWriteAmplitudeToDac();

}
