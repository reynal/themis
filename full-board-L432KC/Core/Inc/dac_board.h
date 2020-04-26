/*
 * dac_board.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_DAC_BOARD_H_
#define INC_DAC_BOARD_H_

#include "misc.h"
#include "stm32l4xx_hal.h"

// ADSR timer
//#define TIM1_INPUT_FREQ 108000000.0
#define TIMDAC_INPUT_FREQ 101000000.0 // test for ADSR algorithms (real value is 108e6)
#define TIMDAC_PRESCALER 100.0
#define TIMDAC_PERIOD 50.0
#define TIMDAC_PERIOD_SEC	((TIMDAC_PRESCALER+1) * TIMDAC_PERIOD/TIMDAC_INPUT_FREQ) // 21386.13Hz, aka 47us


// for Timer 2, see vco_calibration.h



typedef void (*dacTIMUpdateTask)(void); // see dacTIMCallback()


/* Private function prototypes -----------------------------------------------*/

void initSynthParams();

void testGPIOVcfVco();


void dacTIMCallback();
void start_DAC_Timer();
void stop_DAC_Timer();

void blueButtonActionPerformedCallback(GPIO_PinState  state);

void dac_Board_TIM_IRQ(void);
void dac_Board_Init();

void test_Dac_Board();


#endif /* INC_DAC_BOARD_H_ */
