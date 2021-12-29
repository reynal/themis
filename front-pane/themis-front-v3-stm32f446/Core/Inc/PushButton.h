/*
 * PushButton.h
 *
 *  Created on: Nov 22, 2020
 *      Author: sydxrey
 */

#ifndef SRC_PUSHBUTTON_H_
#define SRC_PUSHBUTTON_H_

#include "stm32f4xx_hal.h"
#include "MCP23017.h"
#include <string>

class PushButton {

public:
	PushButton(std::string name, MCP23017::Pin pin);
	~PushButton();

	enum State {
		PUSHED,
		RELEASED
	};

	void stateChanged(State s);

	void printState();

	uint8_t pin;

	State state;

	bool changePending; // raised by stateChanged, must be cleared from client (other approach is to use listeners like in Java)

private:

	uint32_t time;


	std::string name;
};

#endif /* SRC_PUSHBUTTON_H_ */
