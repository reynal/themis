/*
 * ad5391.h
 *
 *  Created on: Apr 16, 2020
 *      Author: sydxrey
 */

#ifndef INC_AD5391_H_
#define INC_AD5391_H_

#include "stm32l4xx_hal.h"

#define MCP23017_REG_IODIRA		0x00
#define MCP23017_REG_IODIRB		0x01
#define MCP23017_REG_GPIOA		0x12
#define MCP23017_REG_GPIOB		0x13
#define MCP23017_IODIR_ALL_OUTPUT	0x00

#define MCP23017_ADDRESS 0x40 // = 0x20 shift one bit to the left

void ad5391_TIM_IRQ(void);
void init_Dac_Board();

void test_AD5391_Spi_Transmit();
void test_AD5391_Spi_Transmit_DMA();
void test_AD5391_Spi_Transmit_DMA_ManualConfig();
void test_AD5391_Dac();
void test_AD5391_Dac_With_Tim();
void test_MCP23017();


#endif /* INC_AD5391_H_ */
