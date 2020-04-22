/*
 * mcp4822.h
 *
 * Low level driver for MCP4822 dac.
 *
 *  Created on: Dec 14, 2019
 *      Author: reynal
 */

#ifndef MCP4822_H_
#define MCP4822_H_

#include "dac_board.h"


// defines for the MCP4822 device
#define MCP4822_CHANNEL_A 			0x30   // 0011 A\/B=0 RES=0 GA\=1 SHDN\=1
#define MCP4822_CHANNEL_B 			0xB0   // 1011
#define MCP4822_CHANNEL_A_GAIN2 	0x10   // 0001 A\/B=0 RES=0 GA\=0 SHDN\=1
#define MCP4822_CHANNEL_B_GAIN2 	0x90   // 1001




/* Private function prototypes -----------------------------------------------*/

void spiDacMCP4822TxCpltCallback();
void dacWrite(int word12bits, Dac targetDac);
void testDacSelect();
void testDacWriteSPI();


#endif /* MCP4822_H_ */
