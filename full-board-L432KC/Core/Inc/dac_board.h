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

#define SWITCH_DEBOUNCE_DELAY 30 // ms

// for Timer 2, see vco_calibration.h


/* Private function prototypes -----------------------------------------------*/

void dac_Board_Timer_Start();
void dac_Board_Timer_Stop();

void dac_Board_Timer_IRQ(void);
void dac_Board_Start();

void dac_Board_EXTI_IRQHandler_SW1();
void dac_Board_EXTI_IRQHandler_SW2();


#endif /* INC_DAC_BOARD_H_ */
