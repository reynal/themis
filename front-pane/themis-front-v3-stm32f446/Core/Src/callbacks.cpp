/*
 * callbacks.c
 *
 *  Created on: Dec 29, 2021
 *      Author: sydxrey
 */

#include "stm32f4xx_hal.h"
#include "MCP23017.h"
#include "main.h"

//extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern MCP23017 mcp101;

// our own implementation of putchar used by printf, so that printf is forwarded
// to the Virtual Com Port (need Hyperterminal or a dedicated terminal on the PC or Mac host station)
//
// Note that when compiling as C++ (i.e. file has extension cpp),
// we need to prepend with extern "C" otherwise the
// function never gets called... (probably because the library was compiled as a C file)

extern "C" {

int __io_putchar(int ch) {

	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 0xFFFF); // beware blocking call!
	return ch;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	__NOP();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {

	__NOP();
}

// ------------------------------ HAL Callback -----------------------------------

/*
 * 1) EXTI Interrupt raised
 * 2) identify which MCP (and port A or B) triggered the interrupt
 * 3) scan list of Buttons attached to this MCP port
 * 4) for each button, check if it's the one which triggered the interrupt
 * 5) if YES, call stateChanged on this button
 * 6) add this button to a list of "pending changes"
 */

//extern "C" {
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	switch (GPIO_Pin){
	case MCP_101_INTA_Pin:
		mcp101.interruptACallback();
		break;
	case MCP_101_INTB_Pin:
		mcp101.interruptBCallback();
	}
}

} // extern "C"

