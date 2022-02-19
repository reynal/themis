/*
 * led.h
 *
 *  Created on: Dec 22, 2021
 *      Author: sydxrey
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

#define MAX_LED_DUTY 256

class LED {

public:
	LED(TIM_HandleTypeDef* timer, uint32_t Channel);
	~LED();
	void setDuty(uint16_t pwm100);
	void on();
	void off();
	void toggle();
	void blink();
	void wave();

private:

	void _start();
	TIM_HandleTypeDef* _htim;
	uint32_t _Channel;
	bool _isStarted= false;

};



#endif /* INC_LED_H_ */
