/*
 * leds.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_LEDS_H_
#define INC_LEDS_H_

void switchGreenLED(GPIO_PinState state);
void switchGreenLEDOn();
void switchGreenLEDOff();
void toggleGreenLED();

void switchRedLED(GPIO_PinState state);
void switchRedLEDOn();
void switchRedLEDOff();
void toggleRedLED();


#endif /* INC_LEDS_H_ */
