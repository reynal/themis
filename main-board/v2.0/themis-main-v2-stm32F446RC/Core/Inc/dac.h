/*
 * dac_board.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_DAC_BOARD_H_
#define INC_DAC_BOARD_H_

#include "misc.h"
#include "stm32f4xx_hal.h"

#define	ADSR_TIMER_PERIOD_FACTOR 20

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
