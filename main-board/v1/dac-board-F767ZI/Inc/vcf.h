/*
 * vcf.h
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey
 */

#ifndef VCF_H_
#define VCF_H_

#include "stm32f7xx_hal.h"

// Filter
#define DEF_MIDICC_CUTOFF 32 // cut off
#define MAX_CUTOFF 1.0

#define DEF_MIDICC_RESONANCE 0 // Q
#define MAX_RESONANCE 4095


/* global parameters cutoff and Q influence the shape of the CVF adsr enveloppe */
typedef struct {
  double vcfCutoff; // between 0 and 100%
  double vcfResonance; // between 0 and 127
} GlobalFilterParams;


/* Private function prototypes -----------------------------------------------*/

void setVcfOrder(uint8_t value);
void initVcf();

void updateVcfCutoff();
void updateVcfResonance();

void setVcfResonanceGlobal(uint8_t midivalue);
void setVcfCutoffGlobal(uint8_t midivalue);

void testVcf();

#endif /* VCF_H_ */
