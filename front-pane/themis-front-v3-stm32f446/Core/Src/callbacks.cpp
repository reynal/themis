/*
 * callbacks.c
 *
 *  Created on: Dec 29, 2021
 *      Author: sydxrey
 */

#include "callback.h"
#include "stm32f4xx_hal.h"
#include "MCP23017.h"
#include "main.h"
#include "gpioDebug.h"
#include "front-panel.h"


//extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern MCP23017 mcp101, mcp110, mcp111;
#ifdef SOUTH_BOARD_CONNECTED
extern MCP23017 mcp000, mcp001, mcp010, mcp011, mcp100;
#endif
extern GpioDebug tx2;

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

// ------------------------------ HAL Callback -----------------------------------

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	//__NOP();
	tx2.toggle();

}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {

	__NOP();
}


/*
 * 1) EXTI Interrupt raised
 * 2) identify which MCP (and port A or B) triggered the interrupt
 * 3) scan list of Buttons attached to this MCP port
 * 4) for each button, check if it's the one which triggered the interrupt
 * 5) if YES, call stateChanged on this button
 * 6) add this button to a list of "pending changes"
 */

//extern "C" {
/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	switch (GPIO_Pin){
	case MCP_101_INTA_Pin:
		mcp101.interruptACallback(); // EXTI15_10_IRQHandler (13)
		break;
	case MCP_101_INTB_Pin:
		mcp101.interruptBCallback(); // EXTI0_IRQHandler
		break;
	case MCP_110_INTA_Pin:
		mcp110.interruptACallback(); //EXTI3
		break;
	case MCP_110_INTB_Pin:
		mcp110.interruptBCallback(); // EXTI2
		break;
	case MCP_111_INTA_Pin:
		mcp111.interruptACallback(); // EXTI4
		break;
	case MCP_111_INTB_Pin:
		mcp111.interruptBCallback(); // EXTI5
		break;
	}
}*/

} // extern "C"

void GPIO_EXTI0_Callback(){ // MCP_101_INTB_Pin
	mcp101.interruptBCallback();
}

void GPIO_EXTI1_Callback(){ //
	mcp011.interruptBCallback();
	//__NOP();
}

void GPIO_EXTI2_Callback(){ // MCP_110_INTB_Pin
	mcp110.interruptBCallback();
}

void GPIO_EXTI3_Callback(){ // MCP_110_INTA_Pin
	mcp110.interruptACallback();
}

void GPIO_EXTI4_Callback(){ // MCP_111_INTA_Pin
	mcp111.interruptACallback();
}


// === same IRQ vector for EXTI5 to EXTI9 ===

void GPIO_EXTI5_Callback(){ // MCP_111_INTB_Pin
	mcp111.interruptBCallback();
}

void GPIO_EXTI6_Callback(){ //
	mcp100.interruptBCallback();
}

void GPIO_EXTI7_Callback(){ //
	mcp010.interruptACallback();
}

void GPIO_EXTI8_Callback(){ //
	mcp010.interruptBCallback();
}

void GPIO_EXTI9_Callback(){ //
	 mcp001.interruptACallback();
}


// === same IRQ vector for EXTI10 to EXTI15 ===

void GPIO_EXTI10_Callback(){ //
	mcp001.interruptBCallback();
}

void GPIO_EXTI11_Callback(){ //
	mcp000.interruptBCallback();
}

void GPIO_EXTI12_Callback(){ //
	mcp000.interruptACallback();
}

void GPIO_EXTI13_Callback(){
	mcp101.interruptACallback();
}

void GPIO_EXTI14_Callback(){ //
	mcp011.interruptACallback();
}

void GPIO_EXTI15_Callback(){ //
	mcp100.interruptACallback();
}




