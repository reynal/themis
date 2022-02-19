/*
 * RotaryEncoder.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: sydxrey
 */

#include "RotaryEncoder.h"
//#include "AbstractController.h"
#include "MCP23017.h"
#include "stdio.h"
#include "main.h"

// ------------

RotaryEncoder::RotaryEncoder(std::string _name, MCP23017::Port _port, MCP23017::Pin chA, MCP23017::Pin chB, RotaryEncoder* _next){ // : AbstractController(_name, _next){

	name = _name;
	next = _next;
	port = _port;
	pinA = chA;
	pinB = chB;
	mask = pinA | pinB;

}

RotaryEncoder::~RotaryEncoder() {}

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
	//printf("name=%s pos=%d    dt=%lu       %c\n", name.c_str(), position, dt, (dt > BIG_STEP_DT ? ' ' : '*'));
}


void RotaryEncoder::nextAltFunction(){}

/*
 * @param mcp23017CaptureValue value of the INTCAP register
 */
void RotaryEncoder::update(uint8_t mcp23017CaptureValue){

	uint8_t nextState = 0;
	if ((mcp23017CaptureValue & pinA) != 0) nextState++;  // b01
	if ((mcp23017CaptureValue & pinB) != 0) nextState+=2; // b10

	//printf("%d, Av: %.1X\n", position++, prevNextCode);

	prevNextCode <<= 2; // change "next" state into "previous" state
	prevNextCode |= nextState; // fill "next" state ; note that these new bits should be LSB's !!!
	prevNextCode &= 0x0F; // make sure this is a 4 bit PSNS value
	// printf("Apr:%.1X\n", prevNextCode);
	if (prevNextCode == 0xB) encoderMoved(MOVE_CW);
	else if (prevNextCode == 0x7) encoderMoved(MOVE_CCW);

}

void RotaryEncoder::print() {

	std::string s = "Enc \"" + name + "\": ["+ MCP23017::printPort(port) +  MCP23017::printPin(pinA) + "," + MCP23017::printPort(port) +  MCP23017::printPin(pinB) + "]";

	//printf("Enc: [%s:%s,%s] name=%s pos=%d\n", MCP23017::printPort(port).c_str(), MCP23017::printPin(pinA).c_str(), MCP23017::printPin(pinB).c_str(), name.c_str(), position);
	printf("%s -> %d\n", s.c_str(), position);

}



