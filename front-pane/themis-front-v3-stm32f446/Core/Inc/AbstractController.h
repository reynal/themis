/*
 * AbstractButton.h
 *
 *  Created on: Jan 2, 2022
 *      Author: sydxrey
 */

#ifndef INC_ABSTRACTCONTROLLER_H_
#define INC_ABSTRACTCONTROLLER_H_

#include <string>
#include "MCP23017.h"

/**
 * This is the abstract superclass for PushButton's and RotaryEncoder's,
 * with the ability to build linked lists of controlers attached to an MCP23017 device.
 */
class AbstractController {

protected:

	std::string name;

public:

	/** init with a next item in the LL */
	AbstractController(std::string _name, AbstractController* next);
	~AbstractController();

	/** Update the state of this button from the given MCP23017 capture register (aka INTCAP). */
	virtual void update(uint8_t mcp23017CaptureValue)=0;

	/** for debugging */
	virtual void print()=0;

	/* mask for INTFLAG register ; depends on MCP23017 pins this encoder is attached to */
	uint8_t mask=0;

	/** if TRUE, signals the state of the button has changed (aka listener behaviour) ; must be cleared by client */
	bool changePending=false;

	/** linked list of controllers attached to MCP23017 */
	AbstractController* next;

	/** MCP23017 port this controller is attached to */
	MCP23017::Port port=MCP23017::PORT_A;
};

#endif /* INC_ABSTRACTCONTROLLER_H_ */
