/*
 * leds.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 *
 *
 *      LED numbering:
 *      - 0 => LED 1 (CH1, green led, northwest corner of F446)
 *      - 1 => LED 2 (CH2, blue led, south side of F446)
 *      - 2 => LED 3 (CH3, red led, west side of F446)
 */

#include "main.h"
#include "leds.h"

#define htimLED htim3
extern TIM_HandleTypeDef htimLED;

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


/* User code ---------------------------------------------------------*/

void ledInit(){

	  HAL_TIM_PWM_Start(&htimLED, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htimLED, TIM_CHANNEL_2);
	  HAL_TIM_PWM_Start(&htimLED, TIM_CHANNEL_3);
	  //htimLED.Instance->CCR1 = 0;
	  __HAL_TIM_SET_COMPARE(&htimLED, TIM_CHANNEL_1, 0);
	  __HAL_TIM_SET_COMPARE(&htimLED, TIM_CHANNEL_2, 0);
	  __HAL_TIM_SET_COMPARE(&htimLED, TIM_CHANNEL_3, 0);
}

/* set the duty cycle of the green led
 * @param pwm255 from 0 to 255 (255 = htimLED.Instance->ARR)
 * */
void ledSetDuty(uint8_t led, uint8_t pwm255){
	if (pwm255 < 0) pwm255 = 0;
	else if (pwm255 > MAX_LED_DUTY) pwm255 = MAX_LED_DUTY;
	//htimLED.Instance->CCR1 = pwm256;
	switch (led){
	case LED_GREEN:
		htimLED.Instance->CCR1 = gamma8[pwm255];
		break;
	case LED_BLUE:
		htimLED.Instance->CCR2 = gamma8[pwm255];
		break;
	case LED_RED:
		htimLED.Instance->CCR3 = gamma8[pwm255];
		break;
	}
}

/* switch the given led on
 * @param led 0, 1 or 2 */
void ledOn(uint8_t led){
	ledSetDuty(led, 255);
}

/* switch the given led off */
void ledOff(uint8_t led){
	ledSetDuty(led, 0);
}

/* toggle the given led */
void ledToggle(uint8_t led){
	switch (led){
	case 0:
		htimLED.Instance->CCR1 = 255-htimLED.Instance->CCR1;
		break;
	case 1:
		htimLED.Instance->CCR2 = 255-htimLED.Instance->CCR2;
		break;
	case 2:
		htimLED.Instance->CCR3 = 255-htimLED.Instance->CCR3;
		break;
	}
}

/* blink the given led */
void ledBlink(uint8_t led){
	//for(int i=0; i<5; i++){
		ledOn(led);
		HAL_Delay(30);
		ledOff(led);
		//HAL_Delay(30);
	//}

}

/* test the various onboard leds */
void ledTest(){

	uint8_t pwmR = 0;
	uint8_t pwmG = 0;
	uint8_t pwmB = 0;
	ledInit();

	while(1){

		pwmR+=1;
		pwmG+=2;
		pwmB+=3;
		if (pwmR > MAX_LED_DUTY) pwmR = 0;
		if (pwmG > MAX_LED_DUTY) pwmG = 0;
		if (pwmB > MAX_LED_DUTY) pwmB = 0;
		ledSetDuty(LED_GREEN, pwmG);
		ledSetDuty(LED_BLUE, pwmB);
		ledSetDuty(LED_RED, pwmR);

		//ledToggle(0);
		//ledToggle(1);
		//ledToggle(2);
		HAL_Delay(200);
	}

}





