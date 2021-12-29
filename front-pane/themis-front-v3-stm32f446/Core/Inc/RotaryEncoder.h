/*
 * RotaryEncoder.h
 *
 *  Created on: Nov 15, 2020
 *      Author: sydxrey
 */

#ifndef SRC_ROTARYENCODER_H_
#define SRC_ROTARYENCODER_H_

#include "stm32f4xx_hal.h"
#include "MCP23017.h"
#include <string>

#define BIG_STEP_VAL 8
#define BIG_STEP_DT 70 // ms

#define BOUNCE_DT 10 // ms

#define MAX_POS	127
#define MIN_POS 0

class RotaryEncoder {

public:

	RotaryEncoder(std::string name, MCP23017::Pin pinA, MCP23017::Pin pinB);

	~RotaryEncoder();

	enum Rotary_Direction {
		MOVE_CW,
		MOVE_CCW
	};

	void encoderMoved(Rotary_Direction r);

	uint8_t getMask();

	uint8_t getPinA();

	uint8_t getPinB();

	void updatePrevNextCode(uint8_t);

	bool changePending; // raised by encoderMoved, must be cleared from client (other approach is to use listeners like in Java)

private:

	// PSNS (Prev State, Next State) table: see https://www.best-microcontroller-projects.com/rotary-encoder.html
	// the table contains the increment for every PSNS code
	// for ex, 0001 = "00 -> 01" = CW rotation
	// 1011 = "10 -> 11" = CCW rotation
	// 1100 = noise or bounce
	// etc
	//int rot_enc_table[16]= {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
	uint8_t prevNextCode; // contains a 4 bit PSNS code

	int previousPosition;

	uint32_t time;

	uint8_t pinA;

	uint8_t pinB;

	std::string name;

public:

	uint8_t mask; // mask for INTFLAG register ; depends on MCP23017 pins this encoder is attached to

	int position;
};

#endif /* SRC_ROTARYENCODER_H_ */
