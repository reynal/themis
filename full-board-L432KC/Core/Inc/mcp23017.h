/*
 * mcp23017.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 *
 *
 * GPA0 : 411_IN_PULSE
 * GPA1 : 411_IN_TRI
 * GPA2 : 411_IN_SYNC
 * GPA3 : 411_IN_SAW
 * GPA4 : VCF_2ND_ORDER
 * GPA5 : VCF_4TH_ORDER
 *
 * GPB0 : BD
 * GPB1 : Snare
 * GPB2 : RIM
 * GPB3 : TOM1
 * GPB4 : TOM2
 *
 *
 */

#include "stm32l4xx_hal.h"

#ifndef INC_MCP23017_H_
#define INC_MCP23017_H_

#define MCP23017_REG_IODIRA		0x00
#define MCP23017_REG_IODIRB		0x01
#define MCP23017_REG_GPIOA		0x12
#define MCP23017_REG_GPIOB		0x13
#define MCP23017_IODIR_ALL_OUTPUT	0x00

#define MCP23017_ADDRESS 0x40 // = 0x20 shift one bit to the left


typedef enum {
	MCP23017_GPIOPORT_A,
	MCP23017_GPIOPORT_B
} MCP23017_GPIO_Port;


#define MCP23017_PULSE_3340_GPIO_Port MCP23017_GPIOPORT_A
#define MCP23017_PULSE_3340_Pin GPIO_PIN_0

#define MCP23017_TRI_3340_GPIO_Port MCP23017_GPIOPORT_A
#define MCP23017_TRI_3340_Pin GPIO_PIN_1

#define MCP23017_SYNC_3340_GPIO_Port MCP23017_GPIOPORT_A
#define MCP23017_SYNC_3340_Pin GPIO_PIN_2

#define MCP23017_SAW_3340_GPIO_Port MCP23017_GPIOPORT_A
#define MCP23017_SAW_3340_Pin GPIO_PIN_3

#define MCP23017_VCF_2NDORDER_GPIO_Port MCP23017_GPIOPORT_A
#define MCP23017_VCF_2NDORDER_Pin GPIO_PIN_4

#define MCP23017_VCF_4THORDER_GPIO_Port MCP23017_GPIOPORT_A
#define MCP23017_VCF_4THORDER_Pin GPIO_PIN_5


void mcp23017_Init_Device();
void mcp23017_Tx_GpioA_Buffer_Dma();
void mcp23017_Tx_GpioB_Buffer_Dma();
void mcp23017_Write_Pin(MCP23017_GPIO_Port port, uint16_t GPIO_Pin, GPIO_PinState PinState); // GPIO_PIN_RESET or GPIO_PIN_SET
void mcp23017_Set_Vcf_2ndOrder();
void mcp23017_Set_Vcf_4thOrder();
void mcp23017_Set_Vco_Saw();
void mcp23017_Set_Vco_Tri();
void mcp23017_Set_Vco_Pulse();

#endif /* INC_MCP23017_H_ */
