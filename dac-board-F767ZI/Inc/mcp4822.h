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



// defines for the MCP4822 device
#define MCP4822_CHANNEL_A 			0x30   // 0011 A\/B=0 RES=0 GA\=1 SHDN\=1
#define MCP4822_CHANNEL_B 			0xB0   // 1011
#define MCP4822_CHANNEL_A_GAIN2 	0x10   // 0001 A\/B=0 RES=0 GA\=0 SHDN\=1
#define MCP4822_CHANNEL_B_GAIN2 	0x90   // 1001


/*
 * enumation of available DACs on the board
 */
typedef enum {
	  DAC_VCO_13700_FREQ,            // LM13700 VCO
	  DAC_VCO_3340A_FREQ,   			// CEM3340A VCO: frequency
	  DAC_VCO_3340A_PWM_DUTY,			// CEM3340A VCO: PWM duty cycle
	  DAC_VCO_3340B_FREQ,   			// CEM3340B VCO: frequency
	  DAC_VCO_3340B_PWM_DUTY,			// CEM3340B VCO: PWM duty cycle
	  DAC_VCF_CUTOFF,		// AS3320 cutoff CV
	  DAC_VCF_RES,			// AS3320 resonance CV
	  DAC_V2140D_IN1,		// V2140D quad vca input #1
	  DAC_V2140D_IN2,		// etc
	  DAC_V2140D_IN3,
	  DAC_V2140D_IN4,
	  //DAC_VCO_DIG,              // Digitally generated waveform, not used yet
	  DAC_V2140D_IN5,
	  DAC_V2140D_IN6,
	  DAC_V2140D_IN7,
	  DAC_V2140D_IN8,
	  DAC_NOISE,			// not used yet

	  // the following are aliases to mixer inputs (see function dacWrite for details on how they are being used)
	  DAC_V2140D_3340A_LVL,

	  DAC_V2140D_3340B_PULSE_LVL,
	  DAC_V2140D_3340B_TRI_LVL,
	  DAC_V2140D_3340B_SAW_LVL,

	  DAC_V2140D_13700_TRI_LVL,
	  DAC_V2140D_13700_SQU_LVL,
	  DAC_V2140D_13700_SUBBASS_LVL,

	  DAC_V2140D_FM_LVL,
	  DAC_V2140D_RINGMOD_LVL,
	  DAC_V2140D_SH_LVL,
	  DAC_V2140D_VCA                 // was LM13700 VCA, now carried out by V2140D
} Dac;


/* Private function prototypes -----------------------------------------------*/

void dac4822ABWrite(int word12bits, int chip, int channelAB);
void dac4822AdressSelect(int chipNumber);
void spiDacs_TxCpltCallback();
void dacWrite(int word12bits, Dac targetDac);

void testDacSelect();
void testDacWriteSPI();


#endif /* MCP4822_H_ */
