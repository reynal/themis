/*
 * gpioDebug.cpp
 *
 *  Created on: Mar 19, 2022
 *      Author: sydxrey
 */

#include "gpioDebug.h"
#include "stm32f4xx_hal.h"

GpioDebug::GpioDebug(GPIO_TypeDef* port, uint16_t pin){

	_port = port;
	_pin = pin;

}

GpioDebug::~GpioDebug(){}

void GpioDebug::on(){
	HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);
}

void GpioDebug::off(){
	HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
}

void GpioDebug::toggle(){
	HAL_GPIO_TogglePin(_port, _pin);
}
