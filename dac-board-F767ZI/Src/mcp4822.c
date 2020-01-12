/*
 * mcp4822.c
 *
 * Low-level driver for MCP4822 12bit dual DAC
 *
 *  Created on: Dec 14, 2019
 *      Author: reynal
 *
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32f7xx_hal.h"
#include "mcp4822.h"
#include "main.h"
#include "stdio.h"
#include "adsr.h"
#include "vcf.h"
#include "vco.h"
#include "midi.h"

/* External variables --------------------------------------------------------*/

extern SPI_HandleTypeDef *hspiDacs;
extern MidiNote midiNote;

/* Private variables ---------------------------------------------------------*/

uint8_t txSpiDacsBuff[2]; // two byte format for the 4822 DAC


/* Private function prototypes -----------------------------------------------*/


/**
 * sends the appropriate address to the 74LS128 3-to-8 decoder
 * so that the corresponding MCP4822 chip is activated when CS is asserted later
 */
void dac4822AdressSelect(int chipNumber){

	// selection du CNA: A0=PC4, A1=PB1, A2=PC5 and address=A2.A1.A0
	chipNumber &= 0x07;

	switch (chipNumber){
	case 0:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 1:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	case 4:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 5:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	case 6:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_RESET);
		break;
	case 7:
		HAL_GPIO_WritePin(LS138_A2_GPIO_Port, LS138_A2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A1_GPIO_Port, LS138_A1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LS138_A0_GPIO_Port, LS138_A0_Pin, GPIO_PIN_SET);
		break;
	}
}

/**
 * Write a sample to one of eight possible MCP4822 DAC through the SPI2 bus
 * @param word12bits an unsigned 12 bits word
 * @param one of 8 possible physical chips
 * @param 4822 dac channel MCP4822_CHANNEL_A or MCP4822_CHANNEL_B or MCP4822_CHANNEL_A_GAIN2 or MCP4822_CHANNEL_B_GAIN2 if you want a 0-4096mV range
 */

void dac4822ABWrite(int word12bits, int chip, int channelAB){

	if (word12bits < 0) word12bits = 0;
	word12bits &= 0xFFF;

	// selection du CNA:
	dac4822AdressSelect(chip);

	txSpiDacsBuff[0]=(((word12bits >> 8) & 0x0F)) | channelAB;
	txSpiDacsBuff[1]=(word12bits & 0xFF);

	// PB1 = 0 (=input ENABLE of 74LS138, which indirectly selects the corresponding circuit)
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(hspiDacs, txSpiDacsBuff, 2);

}

/**
 * Callback implementation for the SPI peripheral "end of transfer" interruption
 */
void spiDacs_TxCpltCallback(){

	// raise CS so as to transfer the latch to the DAC output (coz we have LDAC=0)
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);

}

/**
 * Write the given word to the given DAC
 */
void dacWrite(int word12bits, Dac targetDac){

	if (word12bits > 4095) word12bits = 4095;
	else if (word12bits < 0) word12bits = 0;

	switch (targetDac){

	// ------- 13700 -------

		case DAC_VCO_13700_FREQ:
			dac4822ABWrite(word12bits, 0,  MCP4822_CHANNEL_A_GAIN2);
			break;

	// ------- 3340A -------

		case DAC_VCO_3340A_FREQ :
			dac4822ABWrite(word12bits, 3, MCP4822_CHANNEL_A_GAIN2);
			break;

		case DAC_VCO_3340A_PWM_DUTY :
			dac4822ABWrite(word12bits, 3, MCP4822_CHANNEL_B_GAIN2);
			break;

	// ------- 3340B ------- (tout a fait a droite, "en rabe")

		case DAC_VCO_3340B_FREQ :
			dac4822ABWrite(word12bits, 7, MCP4822_CHANNEL_A_GAIN2);
			break;

		case DAC_VCO_3340B_PWM_DUTY :
			dac4822ABWrite(word12bits, 7, MCP4822_CHANNEL_B_GAIN2);
			break;

	// ------- Noise -------

		case DAC_NOISE:
			dac4822ABWrite(word12bits, 0, MCP4822_CHANNEL_B); // same DAC as 13700 (second on the left)
			break;

	// ------- Mixer 1 2 3 4 (southern one, i.e., closer to the STM32 DAC board ; 1 2 3 4 go from south to north) -------

		case DAC_V2140D_3340B_PULSE_LVL:
		case DAC_V2140D_IN3 : // 3340B pulse
			dac4822ABWrite(word12bits, 1, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_IN4 : // not connected yet
			dac4822ABWrite(word12bits, 1, MCP4822_CHANNEL_B);
			break;

		case DAC_V2140D_3340B_TRI_LVL:
		case DAC_V2140D_IN1 : // 3340B triangle
			dac4822ABWrite(word12bits, 2, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_3340B_SAW_LVL:
		case DAC_V2140D_IN2 : // 3340B saw
			dac4822ABWrite(word12bits, 2, MCP4822_CHANNEL_B);
			break;

	// ------- Mixer 5 6 7 8 (northern one, that is, closer to the XLR jack plug ; 5 6 7 8 go from south to north) -------


		case DAC_V2140D_3340A_LVL:
		case DAC_V2140D_IN7 : // GAIN3_B on EAGLE schematic
			dac4822ABWrite(word12bits, 4, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_VCA :
		case DAC_V2140D_IN8:  // GAIN4_B on EAGLE schematic
			dac4822ABWrite(word12bits, 4, MCP4822_CHANNEL_B_GAIN2);
			break;

		case DAC_V2140D_IN5 : // GAIN1_B on EAGLE schematic, not connected yet
			dac4822ABWrite(word12bits, 5, MCP4822_CHANNEL_A);
			break;

		case DAC_V2140D_IN6 : // GAIN2_B on EAGLE schematic, not connected yet
			dac4822ABWrite(word12bits, 5, MCP4822_CHANNEL_B);
			break;


	// ------- VCF -------


		case DAC_VCF_CUTOFF:
			dac4822ABWrite(word12bits, 6, MCP4822_CHANNEL_A);
			break;

		case DAC_VCF_RES :
			dac4822ABWrite(word12bits, 6, MCP4822_CHANNEL_B_GAIN2);
			break;


	// -------- not implemented yet ---------

		case DAC_V2140D_FM_LVL:
			break;
		case DAC_V2140D_SH_LVL:
			break;
		case DAC_V2140D_RINGMOD_LVL:
			break;
		case DAC_V2140D_13700_SQU_LVL:
			break;
		case DAC_V2140D_13700_TRI_LVL:
			break;
		case DAC_V2140D_13700_SUBBASS_LVL:
			break;



	}
}





// --------------------------------------------------------------------------------------------------
//                                     hardware test code
// --------------------------------------------------------------------------------------------------

// test if LS138 decoding works properly (check signals with oscilloscope and LS138 unplugged from support)
void testDacSelect(){

	int dac;
	while(1){
		for(dac=0; dac < 8; dac++){
			dac4822AdressSelect(dac);
			HAL_Delay(1);
		}
		toggleRedLED();
	}
}


// test if SPI5 (DAC's) works properly (check signals with oscilloscope and LS138 now plugged in support)
void testDacWriteSPI(){

	int word12bits;
	int dac=0;
	int i=0;
	while(1){
		//dac4822ABWrite(word12bits, 3, MCP4822_CHANNEL_B);
		//dacWrite(word12bits, DAC_VCO_3340_FREQ);
		//dacWrite(word12bits, DAC_VCO_13700);
		for(word12bits=0; word12bits < 4096; word12bits+=16){
			for(dac=0; dac < 8; dac++){
				//dacWrite(word12bits, DAC_VCO_3340_FREQ);
				dac4822ABWrite(word12bits, dac, MCP4822_CHANNEL_A);
				HAL_Delay(1);
				dac4822ABWrite(word12bits, dac, MCP4822_CHANNEL_B);
				HAL_Delay(1);
			}

		}
		toggleRedLED();
		printf("DAC test %d\n", i++);
	}
}






