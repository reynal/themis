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
#include "TLC59731.h"


#define BIG_STEP_VAL 8
#define BIG_STEP_DT 70 // ms

#define BOUNCE_DT 10 // ms

#define MAX_POS	127
#define MIN_POS 0

#define MAX_ALT_FCNT_COUNT 4

class RotaryEncoder {

public:

	RotaryEncoder(std::string name, int altFunctionCount, TLC59731* ledControler, int ledIndex, MCP23017::Port _port, MCP23017::Pin pinA, MCP23017::Pin pinB, RotaryEncoder* _next);

	~RotaryEncoder();

	enum Rotary_Direction {
		MOVE_CW,
		MOVE_CCW
	};

	void encoderMoved(Rotary_Direction r);

	void updatePosition(uint8_t mcp23017CaptureValue);

	void print();

	/** increment the alternate function that is currently displayed and acted upon by a rotation of the encoder */
	void nextAltFunction();

	/** set the alternate function that is currently displayed and acted upon by a rotation of the encoder, to the given index */
	void setAltFunction(int altFunctionIndex);

	/** update the LED state in the attached LED controler */
	void updateLED();

	/** linked list of controllers attached to MCP23017 */
	RotaryEncoder* next;

	/* mask for INTFLAG register ; depends on MCP23017 pins this encoder is attached to */
	uint8_t mask=0;

	/** if TRUE, signals the state of the button has changed (aka listener behaviour) ; must be cleared by client */
	bool changePending=false;

	/** MCP23017 port this controller is attached to */
	MCP23017::Port port=MCP23017::PORT_A;

	int midiValue[MAX_ALT_FCNT_COUNT]; // up to 4 possible alternate functions (which is already a lot seeing that each has its own colour palette)

private:

	std::string name;

	MCP23017::Pin pinA, pinB; // pin numbers for this encoder channels A and B

	// PSNS (Prev State, Next State) table: see https://www.best-microcontroller-projects.com/rotary-encoder.html
	// the table contains the increment for every PSNS code
	// for ex, 0001 = "00 -> 01" = CW rotation
	// 1011 = "10 -> 11" = CCW rotation
	// 1100 = noise or bounce
	// etc
	//int rot_enc_table[16]= {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
	uint8_t prevNextCode=0; // contains a 4 bit PSNS code

	uint32_t time=0; // used to measure the rotation speed of the encoder and make small or big increases

	int altFunction=0; // each value corresponds to a distinct alternate function associated to this encoder ; alt function changes through a press onto the push button
	int altFunctionCount = MAX_ALT_FCNT_COUNT; // nb of alt functions

	// --- encoder led ---
	TLC59731* ledControler; // device that controls this encoder LED
	int ledIndex; // index in the daisychain of pixels


};

#endif /* SRC_ROTARYENCODER_H_ */
