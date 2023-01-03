/*
 * ad5644.h
 *
 *  Created on: Apr 16, 2020 (was ad5391.h), adapted to ad5644.h Oct 28, 2020
 *      Author: sydxrey
 */

#ifndef INC_AD5644_H_
#define INC_AD5644_H_

#include "stm32f4xx_hal.h"

#define hspi_AD5644 (&hspi2)

#define AD5644_CHANNEL_COUNT 4
#define AD5644_MAX_VAL 0x3FFF


/*
 * enumation of available channels on the AD5644 pin header (must be 4 channels as a whole)
 */
typedef enum {

	  AD5644_VCO_3340A_FREQ, // A
	  AD5644_VCO_3340B_FREQ, // B
	  AD5644_WAVE_GEN,		 // C
	  AD5644_VCA			 // D

} ad5644Channel_e;

void ad5644Init();
void ad5644FlushBufferDma(uint8_t channel);
void ad5644WriteAsync(uint16_t word12bits, ad5644Channel_e channel);
void ad5644WriteBlocking(uint16_t word, ad5644Channel_e channel);
void ad5644Test();


#endif /* INC_AD5644_H_ */
