/*
 * PushButton.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: sydxrey
 */

#include "PushButton.h"
#include "print.h"
#include <stdio.h>

extern char print_buffer[50];

PushButton::PushButton(std::string _name, MCP23017::Pin _pin) {
	pin = _pin;
	name = _name;
}

PushButton::~PushButton() {
}

void PushButton::stateChanged(State s){

	state = s;
	changePending = true;


}

void PushButton::printState(){

	sprintf(print_buffer, "name=%s state=%d\n", name.c_str(), state); printSerial();

}
