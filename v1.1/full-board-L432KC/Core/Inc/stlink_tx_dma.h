/*
 * stlink_dma.h
 *
 *  Created on: Mar 28, 2020
 *      Author: sydxrey
 */

#ifndef INC_STLINK_TX_DMA_H_
#define INC_STLINK_TX_DMA_H_

#include "stm32l4xx_hal.h"
//#include <string.h>
//#include <stdint.h>

#define BUF_MIN(x, y)                   ((x) < (y) ? (x) : (y))
#define BUF_MAX(x, y)                   ((x) > (y) ? (x) : (y))


void stlink_Tx_dma_init();
void stlink_dma_irq_handler();
void stlink_dma_test();
void stlink_dma_send_string(const char*);

#endif /* INC_STLINK_TX_DMA_H_ */
