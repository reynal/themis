/*
 * ad5391.h
 *
 *  Created on: Apr 16, 2020
 *      Author: sydxrey
 */

#ifndef INC_AD5391_H_
#define INC_AD5391_H_

#include "stm32l4xx_hal.h"

#define AD5391_CHANNEL_COUNT 16

/*
 * enumation of available channels on the board (must be 16 channels as a whole)
 */
typedef enum {

	  DAC_VCO_3340B_FREQ,   			// CEM3340B VCO: frequency (ch 0)
	  DAC_VCO_3340B_PWM_DUTY,			// CEM3340B VCO: PWM duty cycle (ch 1)

	  DAC_VCO_13700_FREQ,            	// LM13700 VCO (ch 2)

	  DAC_VCO_3340A_FREQ,   			// CEM3340A VCO: frequency (ch 3)
	  DAC_VCO_3340A_PWM_DUTY,			// CEM3340A VCO: PWM duty cycle (ch 4)

	  DAC_V2140D_IN3,					// V2140D QUAD VCA input #3A (ch 5) aka DAC_V2140D_3340B_PULSE_LVL:
	  DAC_V2140D_IN4,					// V2140D QUAD VCA input #4A (ch 6)
	  DAC_V2140D_IN1,					// V2140D QUAD VCA input #1A (ch 7) aka DAC_V2140D_3340B_TRI_LVL
	  DAC_V2140D_IN2,					// V2140D QUAD VCA input #2A (ch 8) aka DAC_V2140D_3340B_SAW_LVL

	  DAC_V2140D_IN7,					// V2140D QUAD VCA input #3B (ch 9) aka DAC_V2140D_3340A_LVL
	  DAC_V2140D_IN8,					// V2140D QUAD VCA input #4B (ch 10)
	  DAC_V2140D_IN5,					// V2140D QUAD VCA input #1B (ch 11)
	  DAC_V2140D_IN6,					// V2140D QUAD VCA input #2B (ch 12)

	  DAC_VCF_CUTOFF,					// AS3320 cutoff CV (ch 13)
	  DAC_VCF_RES,						// AS3320 resonance CV (ch 14)

	  DAC_VCA	              			// VCA (ch 15) aka "NOISE_GENE" on board

	  // the following are aliases to mixer inputs (see function dacWrite for details on how they are being used)
	  //DAC_V2140D_3340A_LVL,

	  //DAC_V2140D_3340B_PULSE_LVL,
	  //DAC_V2140D_3340B_TRI_LVL,
	  //DAC_V2140D_3340B_SAW_LVL,

	  //DAC_V2140D_13700_TRI_LVL,
	  //DAC_V2140D_13700_SQU_LVL,
	  //DAC_V2140D_13700_SUBBASS_LVL,

	  //DAC_V2140D_FM_LVL,
	  //DAC_V2140D_RINGMOD_LVL,
	  //DAC_V2140D_SH_LVL,

} Dac;

void ad5391_Init_Device();
void ad5391_Write_Dma(uint8_t channel);
void dacWrite(uint16_t word12bits, Dac channel);
void dacWrite_Blocking(uint16_t word, Dac channel);


#endif /* INC_AD5391_H_ */
