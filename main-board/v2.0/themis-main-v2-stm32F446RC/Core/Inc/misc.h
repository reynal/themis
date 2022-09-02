/*
 * misc.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_MISC_H_
#define INC_MISC_H_

#include "stm32f4xx_hal.h"

#define US      * 1
#define MS      * 1000 US

/*typedef enum {
	FALSE,
	TRUE
} Boolean;*/

// used for accessing the ISR and IFCR DMA regs: (but one can access them directly using DMA1->xxx)
typedef struct {
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_REG;


#endif /* INC_MISC_H_ */
