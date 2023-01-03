/*
 * RotaryEncoder.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: sydxrey
 */

#include "RotaryEncoder.h"
#include "MCP23017.h"
#include "TLC59731.h"
#include "stdio.h"
#include "main.h"
#include "misc.h"



/**
 * @param _name name of the component, mostly for debugging purpose
 * @param _port MCP23017 port (A or B) this encoder is connected to
 * @param _chA MCP23017 pin number the channel A of this encoder is connected to
 * @param _chB MCP23017 pin number the channel B of this encoder is connected to
 * @param _next pointer to the next encoder in the linked list of encoders attached to the same MCP23017 port, or NULL if this is the last button.
 */
RotaryEncoder::RotaryEncoder(std::string _name,
		int altFunctionCount,
		TLC59731* _ledControler,
		int _ledIndex,
		MCP23017::Port _port,
		MCP23017::Pin _chA,
		MCP23017::Pin _chB,
		RotaryEncoder* _next){

	name = _name;
	ledControler = _ledControler;
	ledIndex = _ledIndex;
	next = _next;
	port = _port;
	pinA = _chA;
	pinB = _chB;
	mask = pinA | pinB;

	// TODO:
	for (int i=0; i < MAX_ALT_FCNT_COUNT; i++) midiValue[i]= 64;
}

RotaryEncoder::~RotaryEncoder() {}

/**
 * Called when the encoder has turned, which in turns update the value of the position variable.
 * This method also handles bigger increase/decrease steps when the encoder is rapidly moved.
 */
void RotaryEncoder::encoderMoved(Rotary_Direction direction){

	uint32_t dt = HAL_GetTick()-time;
	if (dt < BOUNCE_DT) return;
	time = HAL_GetTick();
	int p = midiValue[altFunction];
	switch(direction){
		case MOVE_CW :
			if (dt > BIG_STEP_DT) p++;
			else p += BIG_STEP_VAL;
			//position++;
			if (p > 127) p = 127;
			break;
		case MOVE_CCW :
			if (dt > BIG_STEP_DT) p--;
			else p -= BIG_STEP_VAL;
			//position--;
			if (p < 0) p = 0;
			break;
	}
	if (p != midiValue[altFunction]) changePending = true;
	midiValue[altFunction] = p;
	//printf("RE : name=%s pos=%d    dt=%lu       %c\n", name.c_str(), position, dt, (dt > BIG_STEP_DT ? ' ' : '*'));

	//tlc.update(ledIdxLinkedToEncoder, colorTable[position[altFunction]]);
}


void RotaryEncoder::nextAltFunction(){
	altFunction = (altFunction+1) % altFunctionCount;
}

void RotaryEncoder::setAltFunction(int altFunctionIndex){
	altFunction  = altFunctionIndex % altFunctionCount;
}

/**
 * This callback is called whenever the state of this encoder should change as a result of the hosting MCP23017 device having triggered an interrupt
 * @param mcp23017CaptureValue the value of the INTCAP register for the MCP23017 port this encoder is attached to.
 */
void RotaryEncoder::updatePosition(uint8_t mcp23017CaptureValue){

	uint8_t nextState = 0;
	if ((mcp23017CaptureValue & pinA) != 0) nextState++;  // b01
	if ((mcp23017CaptureValue & pinB) != 0) nextState+=2; // b10

	//printf("%d, Av: %.1X\n", position++, prevNextCode);

	prevNextCode <<= 2; // change "next" state into "previous" state
	prevNextCode |= nextState; // fill "next" state ; note that these new bits should be LSB's !!!
	prevNextCode &= 0x0F; // make sure this is a 4 bit PSNS value
	// printf("Apr:%.1X\n", prevNextCode);
	//printf("INTCAPA=%c%c%c%c%c%c%c%c    ", BYTE_TO_BINARY(mcp23017CaptureValue));
	//printf("prevNextCode=%c%c%c%c%c%c%c%c\n", BYTE_TO_BINARY(prevNextCode));
	if (prevNextCode == 0xB) encoderMoved(MOVE_CW);
	else if (prevNextCode == 0x7) encoderMoved(MOVE_CCW);

}

void RotaryEncoder::print() {

	//std::string s = "Enc \"" + name + "\": ["+ MCP23017::printPort(port) +  MCP23017::printPin(pinA) + "," + MCP23017::printPort(port) +  MCP23017::printPin(pinB) + "]";
	//printf("%s -> %d\n", s.c_str(), position[altFunction]);

	printf("Enc '%s:%d' -> %d\n", name.c_str(), altFunction, midiValue[altFunction]);


}

/** update the LED state in the attached LED controler */
void RotaryEncoder::updateLED(){

	if (ledControler == NULL) return;

	switch (altFunction){
	case 0:
		ledControler->update(ledIndex, 0, 0, 2*midiValue[altFunction]);
		break;
	case 1:
		ledControler->update(ledIndex, 2*midiValue[altFunction],0,0);
		break;
	case 2:
		ledControler->update(ledIndex, 0, 2*midiValue[altFunction],0);
		break;
	case 3:
		ledControler->update(ledIndex, 2*midiValue[altFunction], 0, 2*midiValue[altFunction]);
		break;
	}


}



