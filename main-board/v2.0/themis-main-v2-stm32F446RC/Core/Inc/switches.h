/*
 * switches.h
 *
 *  Created on: Aug 12, 2022
 *      Author: sydreynal
 */

#ifndef INC_SWITCHES_H_
#define INC_SWITCHES_H_

#define SWITCH_DEBOUNCE_DELAY 30 // ms

void switch1Pressed();
void switch2Pressed();
void switch1Released();
void switch2Released();
void switchInit();
void switchScan();

#endif /* INC_SWITCHES_H_ */
