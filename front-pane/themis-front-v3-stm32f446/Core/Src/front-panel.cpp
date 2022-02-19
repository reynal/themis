/*
 * front-panel.cpp
 *
 *  Created on: Dec 22, 2021
 *      Author: sydxrey
 */

#include <RGBColor.h>
#include "front-panel.h"
#include "MCP23017.h"
#include "main.h"
#include "RotaryEncoder.h"
#include "PushButton.h"
#include "TLC59731.h"
#include "math.h"
#include "print.h"
#include "led.h"
#include "stdio.h"
#include "string.h"

/**
 * ---- MCP23017 wiring ----
 *
 * NORTH BOARD + add-on board with 3 encoders "ADDON":
 *
 * 	MCP 101 (aka 41 on I2C1)
 * 		RATE.Push	PA0
 * 		SYNC.Push	PA1
 * 		SYNC.Enc	PA2/PA3
 * 		RANGE.Push	PA4
 * 		ADDON1.Enc	PA6/PA5
 *		ADDON3.Push	PA7
 *		ADDON2.Enc	PB0/PB1
 *		ADDON3.Enc	PB2/PB3
 *		ADDON1.Push	PB4
 *		ADDON2.Push	PB5
 * 		RATE.Enc 	PB6/PB7
 *
 * 	MCP 110 (aka 42 on I2C1):
 * 		RANGE.Enc	PA0/PA1
 * 		HUMAN.Enc	PA2/PA3
 * 		HUMAN.Push	PA4
 * 		PATTERN.Enc	PA5/PA6
 * 		PATTERN.Pus	PA7
 * 		DELAY.Enc	PB1/PB0
 * 		Chorus.Enc	PB3/PB2
 * 		Chorus.Push	PB4
 * 		Drive.Enc	PB6/PB5
 * 		Drive.Push	PB7
 *
 * 	MCP 111 (aka 5 on I2C1):
 * 		LATCH.Enc	PA4/PA5
 * 		LATCH.Push	PA6
 * 		BF.Enc		PB0/PB1
 * 		BF.Push		PA7
 * 		ONOFF.Enc	PB3/PB4
 * 		ONOFF.Push	PB2
 * 		PORTA.Enc	PB6/PB7
 * 		PORTA.Push	PB5
 * 		REVERB.Enc	PA2/PA1
 * 		REVERB.Push	PA0
 * 		Delay.Push	PA3
 */

// ------------------------------ defines -----------------------------------

#define MIDI_CC			0xB0    // 176
//#define SOUTH_BOARD_CONNECTED

// ------------------------------ externs -------------------------------------
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;
extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

// ------------------------------ variables -----------------------------------

LED led(&htim3, TIM_CHANNEL_1);

PushButton 		__jeudisoir___ ratePush	("RATE-PUSH", 	MCP23017::PORT_A,	MCP23017::P0, NULL, NULL);

// plaque nord, adresses 101, 110 et 111
MCP23017 mcp101(&hi2c1, MCP23017::ADDR_101);
MCP23017 mcp110(&hi2c1, MCP23017::ADDR_110);
MCP23017 mcp111(&hi2c1, MCP23017::ADDR_111);
// plaque sud-ouest, adresses 000 et 001
MCP23017 mcp000(&hi2c3, MCP23017::ADDR_000);
MCP23017 mcp001(&hi2c3, MCP23017::ADDR_001);
// plaque sud-est, adresses 010, 011 et 100
MCP23017 mcp010(&hi2c2, MCP23017::ADDR_010);
MCP23017 mcp011(&hi2c2, MCP23017::ADDR_011);
MCP23017 mcp100(&hi2c2, MCP23017::ADDR_100);
MCP23017* mcps[3]={&mcp101, &mcp110, &mcp111};

TLC59731 tlcNorth(&hspi2);
TLC59731 tlcSouth1(&hspi1);
TLC59731 tlcSouth2(&hspi3);

// int PushButtonA_count = sizeof(PushButtonA_array)/sizeof(*PushButtonA_array);


// ------------------------------ functions -----------------------------------
void init_hardware() {

	MCP23017::reset(); // reset ALL devices

	mcp101.init();
	mcp110.init();
	mcp111.init();

	// --- MCP 101 ---
	/*RotaryEncoder 	add1Encoder	("ADD1-ENC", 	MCP23017::PORT_A,	MCP23017::P6, MCP23017::P5, NULL);
	PushButton 		add1Push	("ADD1-PUSH", 	MCP23017::PORT_B,	MCP23017::P4, NULL, &add1Encoder);

	RotaryEncoder 	add2Encoder	("ADD3-ENC", 	MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &add1Push);
	PushButton 		add2Push	("ADD2-PUSH", 	MCP23017::PORT_B,	MCP23017::P5, &add2Encoder, &add2Encoder);

	RotaryEncoder 	add3Encoder	("ADD3-ENC", 	MCP23017::PORT_B,	MCP23017::P2, MCP23017::P3, &add2Push);
	PushButton 		add3Push	("ADD3-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, &add1Encoder, &add3Encoder);

	RotaryEncoder 	rateEncoder	("RATE-ENC", 	MCP23017::PORT_B,	MCP23017::P6, MCP23017::P7, &add3Encoder);
	PushButton 		ratePush	("RATE-PUSH", 	MCP23017::PORT_A,	MCP23017::P0, &add3Push, &rateEncoder);

	RotaryEncoder 	syncEncoder	("SYNC-ENC", 	MCP23017::PORT_A,	MCP23017::P2, MCP23017::P3, &ratePush);
	PushButton 		syncPush	("SYNC-PUSH", 	MCP23017::PORT_A,	MCP23017::P1, &syncEncoder, &syncEncoder);

	PushButton 		rangePush	("RANG-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &syncPush, NULL); // TODO : encoder

	mcp101.attachButtonLinkedList(&rangePush, &rateEncoder);*/

	mcp101.attachButtonsA(&ratePush, NULL);


#ifdef SOUTH_BOARD_CONNECTED
	mcp000.init();
	mcp001.init();
	mcp010.init();
	mcp011.init();
	mcp100.init();
#endif


	/*AbstractController *but;
	but = &syncPush;
	//but->print();
	but->update(0);*/

	//TLC59731 tlc1(&hspi1);
	//tlc1.test();
	//TLC59731 tlc2(&hspi2); // plaque nord
	//tlc2.test();
	//TLC59731 tlc3(&hspi3);
	//tlc3.test();
	//tlc.transmitData();

		//printf("%c%c%c%c.%c%c%c%c ", BYTE_TO_BINARY(mcp.read(MCP23017::INTCAPA)));
		//printf("%c%c%c%c.%c%c%c%c\n", BYTE_TO_BINARY(mcp.read(MCP23017::INTCAPB)));

		//mcp.read(MCP23017::GPIO_A); // clear pending interrupts
		//mcp.read(MCP23017::GPIO_B); // clear pending interrupts

}

void main_loop() {

	/*RGBColor rgb;
	 int t=0;
	 int colorIdx0=0;
	 int colorIdx1=0;*/

	while (1) {

		PushButton* push = mcps[0]->buttonLinkedListA;
		while (push != NULL){
			push->print();

			if (push->changePending==true) {
				push->print();
				push->changePending=false;
			}
			push = push->next;
		};


		/*
		 // update color aka Midi CC ?
		 if ((PushButton_array+0)->changePending){
		 if ((PushButton_array+0)->state == PushButton::PUSHED) colorIdx0++;
		 if (colorIdx0 >= color_count) colorIdx0 = 0;
		 (PushButton_array+0)->changePending = false;
		 }
		 if ((PushButton_array+1)->changePending){
		 if ((PushButton_array+1)->state == PushButton::PUSHED) colorIdx1++;
		 (PushButton_array+1)->changePending = false;
		 if (colorIdx1 >= color_count) colorIdx1 = 0;
		 }
		 // send MIDI message?
		 if ((RotaryEncoder_array+0)->changePending){
		 sendMidiMessage(MIDI_CC, cc0[colorIdx0], RotaryEncoder_array[0].position);
		 udpateOLED(RotaryEncoder_array[0].position);
		 (RotaryEncoder_array+0)->changePending = false;
		 }
		 if ((RotaryEncoder_array+1)->changePending){
		 sendMidiMessage(MIDI_CC, cc1[colorIdx1], RotaryEncoder_array[1].position);
		 (RotaryEncoder_array+1)->changePending = false;
		 }

		 // LED #0
		 //double alpha0 = (1 + 0.1 * sin(6.28 * 0.4 * t)) * RotaryEncoder_array[0].position/127.0;
		 double alpha0 = RotaryEncoder_array[0].position/127.0;
		 if (alpha0 > 1.0) alpha0 = 1.0;
		 else if (alpha0 < 0.0) alpha0 = 0.0;
		 rgb.interpolate(colors1[colorIdx0], colors2[colorIdx0], alpha0);
		 tlc.update(0, rgb);

		 // LED #1
		 double alpha1 = (1 + 0.1 * sin(6.28 * 0.5 * t)) * RotaryEncoder_array[1].position/127.0;
		 if (alpha1 > 1.0) alpha1 = 1.0;
		 else if (alpha1 < 0.0) alpha1 = 0.0;
		 rgb.interpolate(colors1[colorIdx1], colors2[colorIdx1], alpha1);
		 tlc.update(1, rgb);

		 tlc.transmitData();
		 t++;
		 */

		/*for (int i=0; i<PushButtonA_count; i++){
		 if ((PushButtonA_array+i)->changePending) {
		 (PushButtonA_array+i)->printState();
		 (PushButtonA_array+i)->changePending = false;
		 }
		 }
		 MCP23017_array[0].read(MCP23017::GPIO_A); // clear pending interrupts

		 for (int i=0; i<PushButtonB_count; i++){
		 if ((PushButtonB_array+i)->changePending) {
		 (PushButtonB_array+i)->printState();
		 (PushButtonB_array+i)->changePending = false;
		 }
		 }
		 MCP23017_array[0].read(MCP23017::GPIO_B); // clear pending interrupts
		 */

		//sprintf(print_buffer, "GPIOA=%d \t GPIOB=%d\n", MCP23017_array[0].read(MCP23017::GPIO_A), MCP23017_array[0].read(MCP23017::GPIO_B) ); printSerial();

		led.blink();
		HAL_Delay(200);
	}
}

