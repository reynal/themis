/*
 * RotaryEncoder.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: sydxrey
 */

#include "RotaryEncoder.h"
#include "MCP23017.h"
#include "stdio.h"
#include "main.h"

extern char print_buffer[50];


// ------------

RotaryEncoder::RotaryEncoder(std::string encoderName, MCP23017::Pin chA, MCP23017::Pin chB){

	name = encoderName;
	pinA = chA;
	pinB = chB;
	mask = pinA | pinB;

}

RotaryEncoder::~RotaryEncoder() {

}

void RotaryEncoder::encoderMoved(Rotary_Direction direction){

	uint32_t dt = HAL_GetTick()-time;
	if (dt < BOUNCE_DT) return;
	time = HAL_GetTick();
	switch(direction){
		case MOVE_CW :
			if (dt > BIG_STEP_DT) position++;
			else position += BIG_STEP_VAL;
			if (position > 127) position = 127;
			break;
		case MOVE_CCW :
			if (dt > BIG_STEP_DT) position--;
			else position -= BIG_STEP_VAL;
			if (position < 0) position = 0;
			break;
	}
	if (position != previousPosition) changePending = true;
	previousPosition = position;

	// DEBUG sprintf(print_buffer, "name=%s pos=%d    dt=%lu       %c\n", name.c_str(), position, dt, (dt > BIG_STEP_DT ? ' ' : '*')); printSerial();

}

uint8_t RotaryEncoder::getMask(){

	return mask;

}

uint8_t RotaryEncoder::getPinA(){

	return pinA;
}

uint8_t RotaryEncoder::getPinB(){

	return pinB;
}

/*
 * @param mcp23017CaptureValue value of the INTCAP register
 */
void RotaryEncoder::updatePrevNextCode(uint8_t mcp23017CaptureValue){

	uint8_t nextState = 0;
	if ((mcp23017CaptureValue & pinA) != 0) nextState++;  // b01
	if ((mcp23017CaptureValue & pinB) != 0) nextState+=2; // b10

	//sprintf(print_buffer, "%d, Av: %.1X\n", position++, prevNextCode); printSerial();

	prevNextCode <<= 2; // change "next" state into "previous" state
	prevNextCode |= nextState; // fill "next" state ; note that these new bits should be LSB's !!!
	prevNextCode &= 0x0F; // make sure this is a 4 bit PSNS value
	//sprintf(print_buffer, "Apr:%.1X\n", prevNextCode); printSerial();
	if (prevNextCode == 0xB) encoderMoved(MOVE_CW);
	else if (prevNextCode == 0x7) encoderMoved(MOVE_CCW);

}



