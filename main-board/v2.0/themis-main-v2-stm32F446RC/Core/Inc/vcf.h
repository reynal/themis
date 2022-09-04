/*
 * vcf.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_VCF_H_
#define INC_VCF_H_

#include "stm32f4xx_hal.h"

// Filter
#define DEF_MIDICC_CUTOFF 32 // cut off
#define MAX_CUTOFF 1.0

#define DEF_MIDICC_RESONANCE 0 // Q
#define MAX_RESONANCE 255


/* global parameters cutoff and Q influence the shape of the CVF adsr enveloppe */
typedef struct {
  double vcfCutoff; // between 0 and 100%
  double vcfResonance; // between 0 and 127
} GlobalFilterParams;


/* Private function prototypes -----------------------------------------------*/

void vcfSetOrder(uint8_t value);
void vcfInit();

void vcfWriteCutoffToDac();
void vcfWriteResonanceToDac();

void vcfSetGlobalResonance(uint8_t midivalue);
void vcfSetGlobalCutoff(uint8_t midivalue);

void vcfTest();


#endif /* INC_VCF_H_ */
