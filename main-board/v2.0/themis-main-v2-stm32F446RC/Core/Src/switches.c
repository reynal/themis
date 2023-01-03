/*
 * switches.c
 *
 * Code for switches SW1 and SW2
 *
 *  Created on: Aug 12, 2022
 *      Author: sydreynal
 */

#include "switches.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "leds.h"
#include "midi.h"

static int sw1_Tick, sw2_Tick; // tick in ms to measure delay in order to debounce switches
static GPIO_PinState sw1_Previous_State, sw2_Previous_State;


void switchInit(){

	sw1_Previous_State = GPIO_PIN_SET;
	sw2_Previous_State = GPIO_PIN_SET;
	sw1_Tick = HAL_GetTick();
	sw2_Tick = HAL_GetTick();
}

void switchScanButtonsState(){

	GPIO_PinState sw1_Current_State = HAL_GPIO_ReadPin(GPIOC, SW1_Pin);
	GPIO_PinState sw2_Current_State  = HAL_GPIO_ReadPin(GPIOC, SW2_Pin);

	if (sw1_Current_State != sw1_Previous_State) { // button changed
		// debounce:
		if (HAL_GetTick() - sw1_Tick < SWITCH_DEBOUNCE_DELAY) return; // return if last change happened recently
		sw1_Tick = HAL_GetTick();
		sw1_Previous_State = sw1_Current_State ;
		if (sw1_Current_State ==GPIO_PIN_RESET) switch1Pressed();
		else switch1Released();
	}
	if (sw2_Current_State != sw2_Previous_State) { // button changed
		// debounce:
		if (HAL_GetTick() - sw2_Tick < SWITCH_DEBOUNCE_DELAY) return; // return if last change happened recently
		sw2_Tick = HAL_GetTick();
		sw2_Previous_State = sw2_Current_State ;
		if (sw2_Current_State ==GPIO_PIN_RESET) switch2Pressed();
		else switch2Released();
	}
}


/**
 * Called when SW1 gets pressed (SW1 is the rightmost button)
 */
void switch1Pressed(){

	printf("SW1 pressed\n");
	//ledOn(LED_BLUE);
	midiProcessIncomingMessage(NOTE_ON, 30,100);

	//debug_counter++;
}

/**
 * Called when SW1 gets released (SW1 is the rightmost button)
 */
void switch1Released(){

	printf("SW1 released\n");
	//ledOff(LED_BLUE);
	midiProcessIncomingMessage(NOTE_OFF, 30, 0);

}


/**
 * Called when SW2 gets pressed (SW2 is the leftmost button)
 */
void switch2Pressed(){

	printf("SW2 pressed\n");

	//int randomNote = rand() % 20 + 34;
	//int randomNote = 30;
	//printf("rd note=%d\n", randomNote);

	//ledOn(LED_RED);
	midiProcessIncomingMessage(NOTE_ON, 50,100);

	// === VCO Calibration mode===
	//vcoCalib_Run(); // busy loop until calibration is over (uncomment when needed)



}

/**
 * Called when SW2 gets released (SW2 is the leftmost button)
 */
void switch2Released(){

	printf("SW2 released\n");
	//ledOff(LED_RED);
	midiProcessIncomingMessage(NOTE_OFF, 50, 0);

}
