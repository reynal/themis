/*
 * dac_board.h
 *
 * A collection of defines and enums for the control of the DAC board + function prototypes
 *
 *  Created on: Oct 1, 2018
 *      Author: S. Reynal
 */

#ifndef DAC_BOARD_H_
#define DAC_BOARD_H_


// timers
#define US      * 1
#define MS      * 1000 US
#define HTIM1_INPUT_FREQ 108000000.0
#define HTIM1_PRESCALER 100.0
#define HTIM1_PERIOD 50.0
// for Timer 2, see vco_calibration.h:
#define TIMER_PERIOD	(HTIM1_PRESCALER * HTIM1_PERIOD/HTIM1_INPUT_FREQ) // 20kHz, 50us
#define	ADSR_TIMER_PERIOD_FACTOR 20
#define	ADSR_TIMER_PERIOD (ADSR_TIMER_PERIOD_FACTOR*TIMER_PERIOD) // 1ms = 0.001
#define	ADSR_TIMER_PERIOD_MS (1000.0*ADSR_TIMER_PERIOD) // around 1ms

// defines for the MCP4822 device
#define MCP4822_CHANNEL_A 		0x30   // 0011 A\/B=0 RES=0 GA\=1 SHDN\=1
#define MCP4822_CHANNEL_B 		0xB0   // 1011
#define MCP4822_CHANNEL_A_GAIN2 	0x10   // 0001 A\/B=0 RES=0 GA\=0 SHDN\=1
#define MCP4822_CHANNEL_B_GAIN2 	0x90   // 1001


//Channels
#define DRUM_CHANNEL 10


/*
 * enumation of available DACs on the board
 */
typedef enum {
	  DAC_VCO_13700,              // LM13700 VCO
	  DAC_VCO_3340_FREQ,   			// CEM3340 VCO: frequency
	  DAC_VCO_3340_PWM_DUTY,			// CEM3340 VCO: PWM duty cycle
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
	  DAC_NOISE,
	  DAC_EN_RABE_A,
	  DAC_EN_RABE_B,
	  // the following are aliases to mixer inputs (see function dacWrite for details on how they are being used)
	  DAC_V2140D_3340_LVL,
	  DAC_V2140D_13700_TRI_LVL,
	  DAC_V2140D_13700_SQU_LVL,
	  DAC_V2140D_13700_SUBBASS_LVL,
	  DAC_V2140D_FM_LVL,
	  DAC_V2140D_RINGMOD_LVL,
	  DAC_V2140D_SH_LVL,
	  DAC_V2140D_VCA                 // was LM13700 VCA, now carried out by V2140D
} Dac;

/* tunable MIDI CC parameters ; parameters are numbered from 0 in the order they are given in the enum */
typedef enum {
	UNUSED_CC,
	OCTAVE_3340, // 0 1 2 3
	WAVE_3340,  // 0 1 2
	PWM_3340,// 0-127
	LEVEL_3340, // 0-127 mixer 1

	SYNC_3340, // 0 ou 127

	OCTAVE_13700, // 0 1 2 3
	DETUNE_13700, // 0-127
	WAVE_13700, // 0-127 entre sq et tri (mixer 2 mixer 3)
	LEVEL_13700, // 0-127 mixer 1

	VCF_CUTOFF,
	VCF_RESONANCE,
	VCF_ORDER,
	VCF_KBDTRACKING,
	VCF_EG,
	VCF_VELOCITY_SENSITIVITY,
	VCF_ATTACK,
	VCF_DECAY,
	VCF_SUSTAIN,
	VCF_RELEASE,

	VCA_VELOCITY_SENSITIVITY,
	VCA_ATTACK,
	VCA_DECAY,
	VCA_SUSTAIN,
	VCA_RELEASE,

	CALIBRATE // send a non zero data to trigger a recalibration

} MidiCCParam;

/* Private function prototypes -----------------------------------------------*/

void dac4822ABWrite(int word12bits, int chip, int channelAB);
void dacSelect(int chipNumber);
void dacWrite(int word12bits, Dac targetDac);
void initSynthParams();
void dacVcaWrite(double amp);
void dacVcfCutoffWrite(double cutoff);
void updateVCAEnveloppeStateMachine();
void updateVCFEnveloppeStateMachine();
void midiNoteOnHandler();
void midiNoteOffHandler();
//void midiFromSpiMessageHandler(uint8_t byte); @deprecated
void playDemo();
void processIncomingMidiMessage(uint8_t status, uint8_t data1, uint8_t data2);
void setSynthParam(uint8_t id, uint8_t value);
void setMidiCCParam(MidiCCParam param, uint8_t value);
void playDrumMachine(uint8_t data1, uint8_t data2);
void updateDrumMachine();
void testDacSelect();
void testDacWriteSPI();
void testGamme();

#endif /* DAC_BOARD_H_ */
