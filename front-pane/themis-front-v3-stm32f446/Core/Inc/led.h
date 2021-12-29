/*
 * led.h
 *
 *  Created on: Dec 22, 2021
 *      Author: sydxrey
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stdint.h"

#define MAX_LED_DUTY 256

class LED {

public:
	LED();
	~LED();
	//void ledInit();
	void setDuty(uint16_t pwm100);
	void on();
	void off();
	void toggle();
	void blink();
	void wave();

private:

};



#endif /* INC_LED_H_ */
