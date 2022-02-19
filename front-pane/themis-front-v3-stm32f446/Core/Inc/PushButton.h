/*
 * PushButton.h
 *
 *  Created on: Nov 22, 2020
 *      Author: sydxrey
 */

#ifndef SRC_PUSHBUTTON_H_
#define SRC_PUSHBUTTON_H_

#include <string>
#include "stm32f4xx_hal.h"

//#include "AbstractController.h"
#include "MCP23017.h"

class RotaryEncoder;

/**
 * There are two types of PushButton's:
 * - lonely push buttons
 * - parts of push-rotary-encoders: in this case, this class contains a reference to the enclosing encoder.
 */
class PushButton { // : public AbstractController {

public:
	/** create a PushButton
	 * @param (optional) target a target RotaryEncoder this PushButton acts upon
	 * @param (optional) the next controller in the linked list of controllers attached to an MCP23017
	 */
	PushButton(std::string _name, MCP23017::Port _port, MCP23017::Pin _pin,  PushButton* _next, RotaryEncoder *_target);

	~PushButton();

	enum State {
		PUSHED,
		RELEASED
	};

	void update(uint8_t mcp23017CaptureValue);

	void print() ;

	/** the state of this button */
	State state = RELEASED;

	/** linked list of controllers attached to MCP23017 */
	PushButton* next;

	/* mask for INTFLAG register ; depends on MCP23017 pins this encoder is attached to */
	uint8_t mask=0;

	/** if TRUE, signals the state of the button has changed (aka listener behaviour) ; must be cleared by client */
	bool changePending=false;

	/** MCP23017 port this controller is attached to */
	MCP23017::Port port=MCP23017::PORT_A;

private:
	std::string name;


	MCP23017::Pin pin; // the MCP23017 pin this button is connected to
	//uint32_t time; // used for time related behaviors, e.g., a "long" press vs a "short" press
	RotaryEncoder* hostingRotaryEncoder; // (optional) the hosting rotary encoder
};

#endif /* SRC_PUSHBUTTON_H_ */
