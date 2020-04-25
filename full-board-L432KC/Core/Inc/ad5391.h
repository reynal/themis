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

void ad5391_Board_TIM_IRQ(void);
void ad5391_Board_Init();
void start_DAC_Timer();
void stop_DAC_Timer();

void ad5391_Test_Board();

#endif /* INC_AD5391_H_ */
