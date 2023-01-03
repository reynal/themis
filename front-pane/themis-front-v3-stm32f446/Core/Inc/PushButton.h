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

#include "MCP23017.h"
#include "RotaryEncoder.h"

//class RotaryEncoder; // useless now that we can include the corresponding header (no circular references anymore)

/**
 * There are two types of PushButton's:
 * - lonely push buttons
 * - parts of push-rotary-encoders: in this case, this class contains a reference to the enclosing encoder.
 */
class PushButton { // : public AbstractController {

public:
	/** constructor for a PushButton that is hosted by a rotary encoder
	 * @param next next button in the list of buttons attached to the same MCP23017 channel
	 */
	PushButton(std::string _name, MCP23017::Port _port, MCP23017::Pin _pin,  PushButton* _next, RotaryEncoder *_target);

	/**
	 * constructor for an isolated PushButton that has its own LED (and as result, no hosting encoder)
	 * @param next next button in the list of buttons attached to the same MCP23017 channel
	 */
	PushButton(std::string _name, TLC59731* ledControler, int ledIndex, MCP23017::Port _port, MCP23017::Pin _pin,  PushButton* _next);

	~PushButton();

	enum State {
		PUSHED,
		RELEASED
	};

	void updateState(uint8_t mcp23017CaptureValue);

	void print() ;

	/** update the LED state in the attached LED controler */
	void updateLED();


	/** the state of this button */
	State state = RELEASED;

	/** linked list of controllers attached to MCP23017 */
	PushButton* next;

	/* mask for INTFLAG register ; depends on MCP23017 pins this encoder is attached to */
	uint8_t mask=0;

	/** if TRUE, signals the state of the button has changed (aka listener behaviour) ; must be cleared by client */
	bool changePending=false;

	/** MCP23017 port this button is attached to */
	MCP23017::Port port;

	/** if this button is not hosted by an encoder, it has its own MIDI value */
	int midiValue;

private:
	std::string name;

	MCP23017::Pin pin; // the MCP23017 pin this button is connected to

	//uint32_t time; // used for time related behaviors, e.g., a "long" press vs a "short" press

	// the hosting rotary encoder if applicable, or NULL if no hosting encoder
	RotaryEncoder* hostingRotaryEncoder;

	// button led if applicable (i.e. if this button is not inside an encoder) or NULL otherwise---
	TLC59731* ledControler; // device that controls this encoder LED
	int ledIndex; // index in the daisychain of pixels

};

#endif /* SRC_PUSHBUTTON_H_ */
