/*
 * led.cpp
 *
 * Drives the green onboard led using TIM1 PWM output.

 *  Created on: Dec 22, 2021
 *      Author: sydxrey
 */
#include "led.h"
#include "stm32f4xx_hal.h"

// TIM3 currently at 133Hz
#define htimLED htim3
extern TIM_HandleTypeDef htimLED;



/* gamma correction array from 8 bit duty cycle to corrected 8 bit duty cycle */
const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

/**
 * Creates a LED object connected to the given timer and PWM channel
 */
LED::LED(TIM_HandleTypeDef* timer, uint32_t Channel){
	_htim = timer;
	_Channel = Channel;
}

LED::~LED(){}

/**
 * Starts the PWM timer with 0% duty cycle.
 */
void LED::_start(){

	  HAL_TIM_PWM_Start_IT(_htim, _Channel); // TIM_CHANNEL_1
	  _htim->Instance->CCR1 = 0;
	  _isStarted = true;
}


/* set the duty cycle of the green led with gamma correction
 * @param pwm256 from 0 to 255 (255 = htimLED.Instance->ARR)
 * */
void LED::setDuty(uint16_t pwm256){
	if (_isStarted==false) _start();
	if (pwm256 < 0) pwm256 = 0;
	else if (pwm256 > MAX_LED_DUTY) pwm256 = MAX_LED_DUTY;
	//htimLED.Instance->CCR1 = pwm256;
	htimLED.Instance->CCR1 = gamma8[pwm256];
}

/* switch the green led on */
void LED::on(){
	if (_isStarted==false) _start();
	htimLED.Instance->CCR1 = 255;
}

/* switch the green led on */
void LED::off(){
	if (_isStarted==false) _start();
	htimLED.Instance->CCR1 = 0;
}

/* switch the green led on */
void LED::toggle(){
	if (_isStarted==false) _start();
	htimLED.Instance->CCR1 = 255 - htimLED.Instance->CCR1;
}

/* sequence of LED blinking */
void LED::blink(){
	if (_isStarted==false) _start();
	for(int i=0; i<5; i++){
		on();
		HAL_Delay(30);
		off();
		HAL_Delay(30);
	}
}

/* breathing like LED */
void LED::wave(){
	if (_isStarted==false) _start();
	int i=0;
	while(i<255){
		setDuty(i++);
		HAL_Delay(3);
	}

	while(i>0){
		setDuty(i--);
		HAL_Delay(15);
	}

}
