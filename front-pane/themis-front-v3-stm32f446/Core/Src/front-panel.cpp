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
#include "misc.h"
#include "gpioDebug.h"

/**
 * Board encoder location:
 *
 * North (west/east):
 *
 * ADDON1 ADDON2 ADDON3 - RATE SYNC RANGE HUMAN  PATTERN LATCH  BACK/F ON/OFF
 *                        -    -    DRIVE CHORUS DELAY   REVERB -      PORTA
 *
 * South:
 *
 *	// EXTI6 : MCP 100 Port B (south)
	// EXTI7 : MCP 010 Port A (south)
	// EXTI8 : MCP 010 Port B (south)
	// EXTI9 : MCP 001 Port A (south)
 *
 * ---- MCP23017 wiring ----
 *
 * NORTH BOARD + add-on board with 3 encoders "ADDON":
 *
 * 	MCP 101 (aka 41 on I2C1) addr=0x4A
 * 	  EXTI13: (aka 10-15)
 * 		RATE.Push	PA0
 * 		SYNC.Push	PA1
 * 		SYNC.Enc	PA2/PA3
 * 		RANGE.Push	PA4
 * 		ADDON1.Enc	PA6/PA5
 *		ADDON3.Push	PA7
 *	  EXTI0:
 *		ADDON2.Enc	PB0/PB1
 *		ADDON3.Enc	PB2/PB3
 *		ADDON1.Push	PB4
 *		ADDON2.Push	PB5
 * 		RATE.Enc 	PB6/PB7
 *
 * 	MCP 110 (aka 42 on I2C1): addr=0x4C
 * 	  EXTI3:
 * 		RANGE.Enc	PA0/PA1
 * 		HUMAN.Enc	PA2/PA3
 * 		HUMAN.Push	PA4
 * 		PATTERN.Enc	PA5/PA6
 * 		PATTERN.Pus	PA7
 * 	  EXTI2:
 * 		DELAY.Enc	PB1/PB0
 * 		Chorus.Enc	PB3/PB2
 * 		Chorus.Push	PB4
 * 		Drive.Enc	PB6/PB5
 * 		Drive.Push	PB7
 *
 * 	MCP 111 (aka 5 on I2C1): addr=0x4E
 * 	  EXTI4:
 * 		REVERB.Push	PA0
 * 		REVERB.Enc	PA2/PA1
 * 		Delay.Push	PA3
 * 		LATCH.Enc	PA4/PA5
 * 		LATCH.Push	PA6
 * 		BF.Push		PA7
 * 	  EXTI5: (aka 5-9)
 * 		BF.Enc		PB0/PB1
 * 		ONOFF.Push	PB2
 * 		ONOFF.Enc	PB3/PB4
 * 		PORTA.Push	PB5
 * 		PORTA.Enc	PB6/PB7
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
GpioDebug tx2(GPIOA, U2_TX_Pin);
GpioDebug rx2(GPIOA, U2_RX_Pin);

// ===== north board MCP_110 (EXTI_3 / EXTI_2) =====

RotaryEncoder 	patternEncoder("PTRN-ENC", 	MCP23017::PORT_A,	MCP23017::P6, MCP23017::P5, NULL);
RotaryEncoder 	humanEncoder("HUMN-ENC", 	MCP23017::PORT_A,	MCP23017::P3, MCP23017::P2, &patternEncoder);
RotaryEncoder 	rangeEncoder("RNGE-ENC", 	MCP23017::PORT_A,	MCP23017::P1, MCP23017::P0, &humanEncoder);

RotaryEncoder 	driveEncoder("DRV-ENC", 	MCP23017::PORT_B,	MCP23017::P5, MCP23017::P6, NULL);
RotaryEncoder 	chorusEncoder("CHOR-ENC", 	MCP23017::PORT_B,	MCP23017::P2, MCP23017::P3, &driveEncoder);
RotaryEncoder 	delayEncoder("DLY-ENC", 	MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &chorusEncoder);

PushButton 		patternPush	("PATTERN-PUSH",MCP23017::PORT_A,	MCP23017::P7, NULL, 		&patternEncoder);
PushButton 		humanPush	("HUMAN-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &patternPush, &humanEncoder);

PushButton 		drivePush	("DRIVE-PUSH", 	MCP23017::PORT_B,	MCP23017::P7, NULL, 		&driveEncoder);
PushButton 		chorusPush	("CHORUS-PUSH", MCP23017::PORT_B,	MCP23017::P4, &drivePush,	&chorusEncoder);

MCP23017 mcp110(&hi2c1, MCP23017::ADDR_110, &humanPush, &rangeEncoder, &chorusPush, &delayEncoder);

// ===== north board MCP_101 (EXTI_13 / EXTI_0) =====

RotaryEncoder 	add1Encoder	("ADD1-ENC", 	MCP23017::PORT_A,	MCP23017::P5, MCP23017::P6, NULL);
RotaryEncoder 	syncEncoder	("SYNC-ENC", 	MCP23017::PORT_A,	MCP23017::P3, MCP23017::P2, &add1Encoder);

RotaryEncoder 	rateEncoder	("RATE-ENC", 	MCP23017::PORT_B,	MCP23017::P7, MCP23017::P6, NULL);
RotaryEncoder 	add3Encoder	("ADD3-ENC", 	MCP23017::PORT_B,	MCP23017::P3, MCP23017::P2, &rateEncoder);
RotaryEncoder 	add2Encoder	("ADD2-ENC", 	MCP23017::PORT_B,	MCP23017::P1, MCP23017::P0, &add3Encoder);

PushButton 		add3Push	("ADD3-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, NULL, 	 	&add3Encoder);
PushButton 		rangePush	("RNGE-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &add3Push, 	&rangeEncoder);
PushButton 		syncPush	("SYNC-PUSH", 	MCP23017::PORT_A,	MCP23017::P1, &rangePush, 	&syncEncoder);
PushButton 		ratePush	("RATE-PUSH", 	MCP23017::PORT_A,	MCP23017::P0, &syncPush, 	&rateEncoder);

PushButton 		add2Push	("ADD2-PUSH", 	MCP23017::PORT_B,	MCP23017::P5, NULL, 		&add2Encoder);
PushButton 		add1Push	("ADD1-PUSH", 	MCP23017::PORT_B,	MCP23017::P4, &add2Push,	&add1Encoder);

MCP23017 mcp101(&hi2c1, MCP23017::ADDR_101, &ratePush, &syncEncoder, &add1Push, &add2Encoder);

// ===== north board MCP_111 (EXTI_4 / EXTI_5) =====

RotaryEncoder 	latchEncoder	("LTCH-ENC", 	MCP23017::PORT_A,	MCP23017::P5, MCP23017::P4, NULL);
RotaryEncoder 	reverbEncoder	("REV-ENC", 	MCP23017::PORT_A,	MCP23017::P1, MCP23017::P2, &latchEncoder);

RotaryEncoder 	portaEncoder	("PORT-ENC", 	MCP23017::PORT_B,	MCP23017::P7, MCP23017::P6, NULL);
RotaryEncoder 	onoffEncoder	("ONOF-ENC", 	MCP23017::PORT_B,	MCP23017::P4, MCP23017::P3, &portaEncoder);
RotaryEncoder 	backfEncoder	("BF-ENC", 		MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &onoffEncoder);

PushButton 		backfPush	("BACKF-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, NULL, 		&backfEncoder);
PushButton 		latchPush	("LATCH-PUSH", 	MCP23017::PORT_A,	MCP23017::P6, &backfPush, 	&latchEncoder);
PushButton 		delayPush	("DELAY-PUSH", 	MCP23017::PORT_A,	MCP23017::P3, &latchPush, 	&delayEncoder);
PushButton 		reverbPush	("REVERB-PUSH", MCP23017::PORT_A,	MCP23017::P0, &delayPush, 	&reverbEncoder);

PushButton 		portaPush	("PORTA-PUSH", 	MCP23017::PORT_B,	MCP23017::P5, NULL, 		&portaEncoder);
PushButton 		onoffPush	("ONOFF-PUSH", 	MCP23017::PORT_B,	MCP23017::P2, &portaPush, 	&onoffEncoder);

MCP23017 mcp111(&hi2c1, MCP23017::ADDR_111, &reverbPush, &reverbEncoder, &onoffPush, &backfEncoder);




// south board, western part, address 000 & 001
//MCP23017 mcp000(&hi2c3, MCP23017::ADDR_000);
//MCP23017 mcp001(&hi2c3, MCP23017::ADDR_001);
// south board, easter part, address 010, 011 & 100
//MCP23017 mcp010(&hi2c2, MCP23017::ADDR_010);
//MCP23017 mcp011(&hi2c2, MCP23017::ADDR_011);
//MCP23017 mcp100(&hi2c2, MCP23017::ADDR_100);

MCP23017* mcps[3]={&mcp101, &mcp110, &mcp111};



// =========
TLC59731 tlcNorth(&hspi2);
TLC59731 tlcSouth1(&hspi1);
TLC59731 tlcSouth2(&hspi3);

// int PushButtonA_count = sizeof(PushButtonA_array)/sizeof(*PushButtonA_array);

static void checkPushButtonChanged(PushButton*);
static void checkEncoderChanged(RotaryEncoder*);

// ------------------------------ functions -----------------------------------
/**
 * Some initialization stuff can't be carried out before this point because the
 * underlying hardware is not init'd yet (e.g., I2C, timers, etc), so that
 * it's useless trying to move the code inside MCP23017.init() to the constructor alas...
 */
void init_hardware() {

	MCP23017::reset(); // reset ALL devices

	// north board:
	mcp101.init(); // 0x4A
	mcp110.init(); // 0x4C
	mcp111.init(); // 0x4E

#ifdef SOUTH_BOARD_CONNECTED
	mcp000.init();
	mcp001.init();
	mcp010.init();
	mcp011.init();
	mcp100.init();
#endif


	//printf("%c%c%c%c.%c%c%c%c ", BYTE_TO_BINARY(mcp.read(MCP23017::INTCAPA)));
	//printf("%c%c%c%c.%c%c%c%c\n", BYTE_TO_BINARY(mcp.read(MCP23017::INTCAPB)));

	//mcp.read(MCP23017::GPIO_A); // clear pending interrupts
	//mcp.read(MCP23017::GPIO_B); // clear pending interrupts

	mcp101.printAttachedControllers();
	mcp110.printAttachedControllers();
	mcp111.printAttachedControllers();

	tlcNorth.test();
}

/**
 * @param push root of the linked list of buttons
 */
static void checkPushButtonChanged(PushButton* push){

	while (push != NULL){
		//push->print();

		if (push->changePending==true) {
			push->print();
			push->changePending=false;
		}
		push = push->next;
	};

}

/**
 * @param re root of the linked list of rotary encoders
 */
static void checkEncoderChanged(RotaryEncoder* re){

	while (re != NULL){
		//re->print();

		if (re->changePending==true) {
			re->print();
			re->changePending=false;
		}
		re = re->next;
	};

}

// timing:
// * 7us for the whole loop when no button pressed or rotated
// * ~3ms for the whole loop when one button pressed or rotated
static void checkEvent(){

	//tx2.on();
	for (int i=0; i<3; i++){
		checkPushButtonChanged(mcps[i]->buttonLinkedListA);
		checkPushButtonChanged(mcps[i]->buttonLinkedListB);
		checkEncoderChanged(mcps[i]->encoderLinkedListA);
		checkEncoderChanged(mcps[i]->encoderLinkedListB);
	};
	//tx2.off();
}

void main_loop() {

	/*RGBColor rgb;
	 int t=0;
	 int colorIdx0=0;
	 int colorIdx1=0;*/

	int count=0;
	int intensity=0;

	while (1) {

		//printf("%d\n", count++);
		checkEvent();

		intensity += 100;
		intensity %= 200;
		for (int led=0; led < TLC_LED_COUNT; led++)  {
			tlcNorth.update(led, 0, 0, intensity);
		}
		tlcNorth.transmitData();

		/*printf("MCP101: A=%c%c%c%c.%c%c%c%c B=%c%c%c%c.%c%c%c%c ", BYTE_TO_BINARY(mcp101.read(MCP23017::GPIO_A)),BYTE_TO_BINARY(mcp101.read(MCP23017::GPIO_B)));
		printf("MCP110: A=%c%c%c%c.%c%c%c%c B=%c%c%c%c.%c%c%c%c ", BYTE_TO_BINARY(mcp110.read(MCP23017::GPIO_A)),BYTE_TO_BINARY(mcp110.read(MCP23017::GPIO_B)));
		printf("MCP111: A=%c%c%c%c.%c%c%c%c B=%c%c%c%c.%c%c%c%c\n", BYTE_TO_BINARY(mcp111.read(MCP23017::GPIO_A)),BYTE_TO_BINARY(mcp111.read(MCP23017::GPIO_B)));*/

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
		tx2.toggle();
		HAL_Delay(300);
	}
}

