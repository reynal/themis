/*
 * RotaryEncoder.h
 *
 *  Created on: Nov 15, 2020
 *      Author: sydxrey
 */

#ifndef SRC_ROTARYENCODER_H_
#define SRC_ROTARYENCODER_H_


#include <string>
#include "stm32f4xx_hal.h"

#include "MCP23017.h"
//#include "AbstractController.h"


#define BIG_STEP_VAL 8
#define BIG_STEP_DT 70 // ms

#define BOUNCE_DT 10 // ms

#define MAX_POS	127
#define MIN_POS 0

class RotaryEncoder { // : public AbstractController {

public:

	RotaryEncoder(std::string name, MCP23017::Port _port, MCP23017::Pin pinA, MCP23017::Pin pinB, RotaryEncoder* _next);

	~RotaryEncoder();

	enum Rotary_Direction {
		MOVE_CW,
		MOVE_CCW
	};

	void encoderMoved(Rotary_Direction r);

	void update(uint8_t mcp23017CaptureValue);

	void print();

	void nextAltFunction();

	/** linked list of controllers attached to MCP23017 */
	RotaryEncoder* next;

	/* mask for INTFLAG register ; depends on MCP23017 pins this encoder is attached to */
	uint8_t mask=0;

	/** if TRUE, signals the state of the button has changed (aka listener behaviour) ; must be cleared by client */
	bool changePending=false;

	/** MCP23017 port this controller is attached to */
	MCP23017::Port port=MCP23017::PORT_A;

private:

	std::string name;


	// PSNS (Prev State, Next State) table: see https://www.best-microcontroller-projects.com/rotary-encoder.html
	// the table contains the increment for every PSNS code
	// for ex, 0001 = "00 -> 01" = CW rotation
	// 1011 = "10 -> 11" = CCW rotation
	// 1100 = noise or bounce
	// etc
	//int rot_enc_table[16]= {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
	uint8_t prevNextCode=0; // contains a 4 bit PSNS code

	int previousPosition=0; // a way to check if button position has changed and to send a message if any

	uint32_t time=0;

	MCP23017::Pin pinA;

	MCP23017::Pin pinB;

	int altFunction=0; // each value corresponds to a distinct alternate function associated to this encoder ; alt function changes through a press onto the push button

	int altFunctionCount; // nb of alt functions

public:

	int position=0;

};

#endif /* SRC_ROTARYENCODER_H_ */
