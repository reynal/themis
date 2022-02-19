/*
 * PushButton.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: sydxrey
 */

#include "PushButton.h"

#include "print.h"
#include <stdio.h>

PushButton::PushButton(std::string _name, MCP23017::Port _port, MCP23017::Pin _pin, PushButton* _next, RotaryEncoder* _hostingRotaryEncoder){ // : AbstractController(_name, _next) {

	name = _name;
	next = _next;
	pin = _pin;
	port = _port;
	mask = pin;
	hostingRotaryEncoder = _hostingRotaryEncoder;
}

PushButton::~PushButton() {}

void PushButton::update(uint8_t mcp23017CaptureValue) {

	state = (mcp23017CaptureValue & pin) == 0 ? PushButton::RELEASED : PushButton::PUSHED;
	changePending = true;
	// TODO : if hostingRotaryEncoder != NULL

}


void PushButton::print() {

	std::string s = "Push \"" + name + "\": ["+ MCP23017::printPort(port) +  MCP23017::printPin(pin) + "]";
	//printf("Push: [%s:%d] name=%s state=%d\n", (port == MCP23017::PORT_A ? "A":"B"), pin, name.c_str(), state);
	printf("%s -> %s\n", s.c_str(), state == PUSHED ? "PUSHED" : "RELEASED");
}

