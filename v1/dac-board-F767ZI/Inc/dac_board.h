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

#include "misc.h"
#include "stm32f7xx_hal.h"

// ADSR timer
//#define TIM1_INPUT_FREQ 108000000.0
#define TIMDAC_INPUT_FREQ 101000000.0 // test for ADSR algorithms (real value is 108e6)
#define TIMDAC_PRESCALER 100.0
#define TIMDAC_PERIOD 50.0
#define TIMDAC_PERIOD_SEC	((TIMDAC_PRESCALER+1) * TIMDAC_PERIOD/TIMDAC_INPUT_FREQ) // 21386.13Hz, aka 47us


// for Timer 2, see vco_calibration.h

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


typedef void (*dacTIMUpdateTask)(void); // see dacTIMCallback()


/* Private function prototypes -----------------------------------------------*/

void initSynthParams();

void testGPIOVcfVco();

void switchRedLED(GPIO_PinState state);
void switchRedLEDOn();
void switchRedLEDOff();
void toggleRedLED();

void switchBlueLED(GPIO_PinState state);
void switchBlueLEDOn();
void switchBlueLEDOff();
void toggleBlueLED();

void dacTIMCallback();
void startDacTIM();
void stopDacTIM();

void blueButtonActionPerformedCallback(GPIO_PinState  state);

#endif /* DAC_BOARD_H_ */
