/*
 * gpioDebug.h
 *
 *  Created on: Mar 19, 2022
 *      Author: sydxrey
 */

#ifndef INC_GPIODEBUG_H_
#define INC_GPIODEBUG_H_

#include "stm32f4xx_hal.h"

class GpioDebug {

public:
	GpioDebug(GPIO_TypeDef* port, uint16_t pin);
	~GpioDebug();
	void on();
	void off();
	void toggle();

private:
	GPIO_TypeDef* _port;
	uint16_t _pin;

};



#endif /* INC_GPIODEBUG_H_ */
