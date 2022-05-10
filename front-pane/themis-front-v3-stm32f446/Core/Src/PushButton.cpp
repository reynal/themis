/*
 * PushButton.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: sydxrey
 */

#include "PushButton.h"

#include "print.h"
#include <stdio.h>

/**
 * @param _name name of the component, mostly for debugging purpose
 * @param _port MCP23017 port (A or B) this button is connected to
 * @param _pin MCP23017 pin number this button is connected to
 * @param _next pointer to the next button in the linked list of buttons attached to the same MCP23017 port, or NULL if this is the last button.
 * @param _hostingRotaryEncoder pointer to a RotaryEncoder whose this button is the "alternate function" trigger
 */
PushButton::PushButton(std::string _name, MCP23017::Port _port, MCP23017::Pin _pin, PushButton* _next, RotaryEncoder* _hostingRotaryEncoder){ // : AbstractController(_name, _next) {

	name = _name;
	next = _next;
	pin = _pin;
	port = _port;
	mask = pin;
	hostingRotaryEncoder = _hostingRotaryEncoder;
}

PushButton::~PushButton() {}

/**
 * This callback is called whenever the state of this button should changed as a result of the hosting MCP23017 device having triggered an interrupt
 * @param mcp23017CaptureValue the value of the INTCAP register for the MCP23017 port this button is attached to.
 */
void PushButton::update(uint8_t mcp23017CaptureValue) {

	state = (mcp23017CaptureValue & pin) == 0 ? PushButton::RELEASED : PushButton::PUSHED;
	changePending = true;
	//print();
	// TODO : if hostingRotaryEncoder != NULL

}

/** debugging method */
void PushButton::print() {

	std::string s = "Push \"" + name + "\": ["+ MCP23017::printPort(port) +  MCP23017::printPin(pin) + "]";
	//printf("Push: [%s:%d] name=%s state=%d\n", (port == MCP23017::PORT_A ? "A":"B"), pin, name.c_str(), state);
	printf("%s -> %s\n", s.c_str(), state == PUSHED ? "PUSHED" : "RELEASED");
}

