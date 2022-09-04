/*
 * bh2221.h
 *
 *  Created on: Oct 28, 2020
 *      Author: sydxrey
 */

#ifndef INC_BH2221_H_
#define INC_BH2221_H_

#include "stm32f4xx_hal.h"

#define hspi_BH2221 (&hspi1)

#define BH2221_CHANNEL_COUNT 12
#define BH2221_MAX_VAL 0x00FF

#define BH2221_AO1 0
#define BH2221_AO2 1
#define BH2221_AO3 2
#define BH2221_AO4 3
#define BH2221_AO5 4
#define BH2221_AO6 5
#define BH2221_AO7 6
#define BH2221_AO8 7
#define BH2221_AO9 8
#define BH2221_AO10 9
#define BH2221_AO11 10
#define BH2221_AO12 11

/*
 * enumation of available channels on the BH2221 pin header (must be 12 channels as a whole)
 *
 * V part (left side of F446):
 * 11 : MIXER 1A
 * 12 : MIXER 4A
 * 1  : PWMB
 * 2  : PWMA
 *
 * H part:
 * 10 9 8 7 3 4 5 6
 * with:
 * 10 : MIXER 3A
 * 9  : MIXER 2A
 * 8  : MIXER 1B
 * 7  : MIXER 4B
 * 3  : MIXER 3B
 * 4  : MIXER 2B
 * 5  : VCF RES
 * 6 : VCF CUTOFF
 *
 *
 * 1A xmod 		= AO11
 * 4A subbass 	= AO12
 * 3A tri B 	= AO10
 * 2A pulse b 	= AO9
 * 1B saw b 	= AO8
 * 4B tri a 	= AO7
 * 3B squ a 	= AO3
 * 2B saw a 	= AO4
 */
typedef enum {

	// A01: PWMB
	  BH2221_VCO_3340B_PWM_DUTY,
	// A02: PWMA
	  BH2221_VCO_3340A_PWM_DUTY,
	// A03: 3B
	  BH2221_V2140D_3340A_PULSE_LVL,
	// A04: 2B
	  BH2221_V2140D_3340A_SAW_LVL,
	// A05: VCF Q
	  BH2221_VCF_RES,
	// A06: VCF Cutoff
	  BH2221_VCF_CUTOFF,
	// A07: 4B
	  BH2221_V2140D_3340A_TRI_LVL,
	// A08: 1B
	  BH2221_V2140D_3340B_SAW_LVL,
	// A09: 2A
	  BH2221_V2140D_3340B_PULSE_LVL,
	// A10: 3A
	  BH2221_V2140D_3340B_TRI_LVL,
	// A11: 1A
	  BH2221_V2140D_XMOD_LVL,
	// A12: 4A
	  BH2221_V2140D_3340B_SUB_LVL,


} bh2221Channel_e;

void bh2221InitDMA();
void bh2221FlushBufferDma(bh2221Channel_e channel);
void bh2221WriteAsync(uint8_t byte, bh2221Channel_e channel);
void bh2221WriteBlocking(uint8_t byte, bh2221Channel_e channel);
void bh2221Test();


#endif /* INC_BH2221_H_ */
