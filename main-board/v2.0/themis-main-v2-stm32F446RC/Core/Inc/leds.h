/*
 * leds.h
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */

#ifndef INC_LEDS_H_
#define INC_LEDS_H_

#define MAX_LED_DUTY 255

#define LED_GREEN 0
#define LED_BLUE 1
#define LED_RED 2

void ledInit();
void ledSetDuty(uint8_t led, uint8_t pwm255);
void ledOn(uint8_t led);
void ledOff(uint8_t led);
void ledToggle(uint8_t led);
void ledBlink(uint8_t led);
void ledTest();


#endif /* INC_LEDS_H_ */
