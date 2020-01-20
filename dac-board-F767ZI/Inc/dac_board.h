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

// timers
//#define HTIM1_INPUT_FREQ 108000000.0
#define HTIM1_INPUT_FREQ 101000000.0 // test for ADSR algorithms (real value is 108e6)
#define HTIM1_PRESCALER 100.0
#define HTIM1_PERIOD 50.0
// for Timer 2, see vco_calibration.h:
#define TIMER_PERIOD	((HTIM1_PRESCALER+1) * HTIM1_PERIOD/HTIM1_INPUT_FREQ) // 21386.13Hz, aka 47us


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
void incDacTick();

void blueButtonActionPerformedCallback(GPIO_PinState  state);

#endif /* DAC_BOARD_H_ */
