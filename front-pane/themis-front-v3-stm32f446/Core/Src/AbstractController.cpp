/*
 * AbstractButton.c
 *
 *  Created on: Jan 2, 2022
 *      Author: sydxrey
 */

#include <AbstractController.h>
#include "stm32f4xx_hal.h"
#include "MCP23017.h"
#include <string.h>

AbstractController::AbstractController(std::string _name, AbstractController* _next) {
	name = _name;
	next = _next;
}

AbstractController::~AbstractController() {}

