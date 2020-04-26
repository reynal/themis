/*
 * stlink_dma.h
 *
 *  Created on: Mar 28, 2020
 *      Author: sydxrey
 */

#ifndef INC_STLINK_DMA_H_
#define INC_STLINK_DMA_H_

#include "stm32l4xx_hal.h"
//#include <string.h>
//#include <stdint.h>

#define BUF_MIN(x, y)                   ((x) < (y) ? (x) : (y))
#define BUF_MAX(x, y)                   ((x) > (y) ? (x) : (y))


size_t stlink_dma_buff_write(const char* data, size_t btw); // TODO make static
size_t stlink_dma_buff_get_writable_size();
int stlink_dma_buff_get_readable_size();
uint8_t * stlink_dma_buff_get_linear_block_read_address();
size_t stlink_dma_buff_get_linear_block_read_length();
int stlink_dma_buff_skip(size_t len);
void stlink_dma_init();
void stlink_dma_transmit();
void stlink_dma_irq_handler();
void stlink_dma_send_string(const char*);

void stlink_dma_test();


#endif /* INC_STLINK_DMA_H_ */
