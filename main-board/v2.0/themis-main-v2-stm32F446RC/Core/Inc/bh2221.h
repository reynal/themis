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
 */
typedef enum {

	  BH2221_VCO_3340A_PWM_DUTY,
	  BH2221_VCO_3340B_PWM_DUTY,

	  BH2221_V2140D_3340B_PULSE_LVL, // BH2221_V2140D_IN1,
	  BH2221_V2140D_3340B_SAW_LVL, // BH2221_V2140D_IN2,
	  BH2221_V2140D_3340B_TRI_LVL, // BH2221_V2140D_IN3,
	  BH2221_V2140D_IN4,

	  BH2221_V2140D_3340A_PULSE_LVL, // BH2221_V2140D_IN5,
	  BH2221_V2140D_3340A_SAW_LVL, // BH2221_V2140D_IN6,
	  BH2221_V2140D_3340A_TRI_LVL, // BH2221_V2140D_IN7,
	  BH2221_V2140D_IN8,

	  BH2221_VCF_CUTOFF,
	  BH2221_VCF_RES,

} bh2221Channel_e;

void bh2221InitDMA();
void bh2221XferBufferDma(bh2221Channel_e channel);
void bh2221WriteDmaBuffer(uint8_t byte, bh2221Channel_e channel);
void bh2221WriteBlocking(uint8_t byte, bh2221Channel_e channel);
void bh2221Test();


#endif /* INC_BH2221_H_ */
