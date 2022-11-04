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

// ------------------------------ defines -----------------------------------

#define MIDI_CC			0xB0    // 176

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
GpioDebug tx2(GPIOA, U2_TX_Pin); // temp use of USART2 as GPIO for debugging
GpioDebug rx2(GPIOA, U2_RX_Pin);

/**
 * Board encoder location:
 *
 * North (west/east):
 *
 * ADDON1 ADDON2 ADDON3 - RATE SYNC RANGE HUMAN  PATTERN LATCH  BACK/F ON/OFF
 *                        -    -    DRIVE CHORUS DELAY   REVERB -      PORTA
 *
 *
 * ---- MCP23017 wiring ----
 *
 * NORTH BOARD + add-on board with 3 encoders "ADDON":
 *
 * 	MCP 101 (aka 41, on I2C1) addr=0x4A
 * 	  EXTI13:
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
 * 	MCP 110 (aka 42, on I2C1): addr=0x4C
 * 	  EXTI3:
 * 		RANGE.Enc	PA0/PA1
 * 		HUMAN.Enc	PA2/PA3
 * 		HUMAN.Push	PA4
 * 		PATTERN.Enc	PA5/PA6
 * 		PATTERN.Pus	PA7
 * 	  EXTI2:
 * 		DELAY.Enc	PB1/PB0
 * 		CHORUS.Enc	PB3/PB2
 * 		CHORUS.Push	PB4
 * 		DRIVE.Enc	PB6/PB5
 * 		DRIVE.Push	PB7
 *
 * 	MCP 111 (aka 5, on I2C1): addr=0x4E
 * 	  EXTI4:
 * 		REVERB.Push	PA0
 * 		REVERB.Enc	PA2/PA1
 * 		DELAY.Push	PA3
 * 		LATCH.Enc	PA4/PA5
 * 		LATCH.Push	PA6
 * 		BF.Push		PA7
 * 	  EXTI5:
 * 		BF.Enc		PB0/PB1
 * 		ONOFF.Push	PB2
 * 		ONOFF.Enc	PB3/PB4
 * 		PORTA.Push	PB5
 * 		PORTA.Enc	PB6/PB7
 *
 *
 *
 * SOUTH BOARD (vco "Dig", "1" and "2")
 *
 *  MCP 000 (aka 11, on I2C3): addr=0x40
 * 	  EXTI12:
 *		MIX1.Enc	 PA0/PA1
 *		MIX1.Push	 PA2
 *		TUNE1.Enc	 PA3/PA4
 *		TUNE1.Push	 PA5
 *		WAVEDIG.Push PA6
 *		OCTDIG.Push	 PA7
 *
 * 	  EXTI11:
 *		WAVEDIG.Enc	 PB0/PB1
 *		TUNEDIG.Enc	 PB2/PB3
 *		TUNEDIG.Push PB4
 *		MIXDIG.Push	 PB5
 *		MIXDIG.Enc	 PB6/PB7
 *
 *  MCP 001 (aka 12, on I2C3): addr=0x42
 *	  EXTI9:
 *	  	MIX2.Enc	PA0/PA2
 *	  	MIX2.Push	PA1
 *		TUNE2.Enc	PA3/PA4
 *		TUNE2.Push	PA5
 *		OCT2.Pusy	PA6
 *		SYNC.Push	PA7
 *
 *	  EXTI10:
 *			!!! PB0 not connected !!!
 *	  	WAVE2.Enc	PB1/PB3
 *	  	WAVE2.Push	PB2
 *	  	WAVE1.Push	PB4
 *		WAVE1.Enc	PB5/PB6
 *		OCT1.Push	PB7
 *
 *  MCP 010 (aka 2, on I2C2): addr=0x44
 *	  EXTI7:
 *		LFO2SYNC.Push	PA0
 *		LFO2RATE.Enc	PA1/PA2
 *		LFO2RATE.Push	PA3
 *		LFO2XXX.Push	PA4
 *		LFO2AMNT.Enc	PA5/PA7
 *		LFO2AMNT.Push	PA6
 *
 *	  EXTI8:
 *		LFO1XXX.Push	PB0
 *		LFO1AMNT.Enc	PB1/PB2
 *		LFO1AMNT.Push	PB3
 *		LFO1RATE.Enc	PB4/PB5
 *		LFO1RATE.Push	PB6
 *		LFO1SYNC.Push	PB7
 *
 *  MCP 011 (aka 31, on I2C2): addr=0x46
 *    EXTI14:
 *		CUTOFF.Enc		PA0/PA1
 *		CUTOFF.Push		PA2
 *		ORDER.Push		PA3
 *		EGAMNT.Push		PA4
 *		EGAMNT.Enc		PA5/PA6
 *		VCFR.Push		PA7
 *
 *    EXTI1:
 *		VCFR.Enc		PB0/PB1
 *		VCFS.Push		PB2
 *		VCFS.Enc		PB3/PB4					// MARCHE PAS !!!
 *		VCFD.Enc		PB5/PB6
 *		VCFD.Push		PB7
 *
 *  MCP 100 (aka 32, on I2C2): addr=0x48
 *    EXTI15:
 *		EG3.Push		PA0
 *		VCAR.Push		PA1
 *		VCAR.Enc		PA2/PA3
 *		VCAS.Push		PA4
 *		VCAS.Enc		PA5/PA6
 *		VCAD.Push		PA7
 *
 *    EXTI6:
 *		VCAD.Enc		PB0/PB1
 *		VCAA.Enc		PB2/PB3
 *		VCAA.Push		PB4
 *		VCFA.Enc		PB5/PB6
 *		VCFA.Push		PB7
 *
 */


// ===== north board MCP_110 (EXTI_3 / EXTI_2) =====

RotaryEncoder 	patternEncoder("PATTRN-ENC",MCP23017::PORT_A,	MCP23017::P6, MCP23017::P5, NULL);
RotaryEncoder 	humanEncoder("HUMAN-ENC", 	MCP23017::PORT_A,	MCP23017::P3, MCP23017::P2, &patternEncoder);
RotaryEncoder 	rangeEncoder("RANGE-ENC", 	MCP23017::PORT_A,	MCP23017::P1, MCP23017::P0, &humanEncoder);

RotaryEncoder 	driveEncoder("DRIVE-ENC", 	MCP23017::PORT_B,	MCP23017::P5, MCP23017::P6, NULL);
RotaryEncoder 	chorusEncoder("CHORUS-ENC",	MCP23017::PORT_B,	MCP23017::P2, MCP23017::P3, &driveEncoder);
RotaryEncoder 	delayEncoder("DELAY-ENC", 	MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &chorusEncoder);

PushButton 		patternPush	("PATTERN-PUSH",MCP23017::PORT_A,	MCP23017::P7, NULL, 		&patternEncoder);
PushButton 		humanPush	("HUMAN-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &patternPush, &humanEncoder);

PushButton 		drivePush	("DRIVE-PUSH", 	MCP23017::PORT_B,	MCP23017::P7, NULL, 		&driveEncoder);
PushButton 		chorusPush	("CHORUS-PUSH", MCP23017::PORT_B,	MCP23017::P4, &drivePush,	&chorusEncoder);

MCP23017 mcp110(&hi2c1, MCP23017::ADDR_110, &humanPush, &rangeEncoder, &chorusPush, &delayEncoder);

// ===== north board MCP_101 (EXTI_13 / EXTI_0) =====

RotaryEncoder 	add1Encoder		("ADD1-ENC", 	MCP23017::PORT_A,	MCP23017::P5, MCP23017::P6, NULL);
RotaryEncoder 	arpSyncEncoder	("ARPSYNC-ENC",	MCP23017::PORT_A,	MCP23017::P3, MCP23017::P2, &add1Encoder);

RotaryEncoder 	rateEncoder	("RATE-ENC", 	MCP23017::PORT_B,	MCP23017::P7, MCP23017::P6, NULL);
RotaryEncoder 	add3Encoder	("ADD3-ENC", 	MCP23017::PORT_B,	MCP23017::P3, MCP23017::P2, &rateEncoder);
RotaryEncoder 	add2Encoder	("ADD2-ENC", 	MCP23017::PORT_B,	MCP23017::P1, MCP23017::P0, &add3Encoder);

PushButton 		add3Push	("ADD3-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, NULL, 	 	&add3Encoder);
PushButton 		rangePush	("RANGE-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &add3Push, 	&rangeEncoder);
PushButton 		arpSyncPush	("ARPSYNC-PUSH",MCP23017::PORT_A,	MCP23017::P1, &rangePush, 	&arpSyncEncoder);
PushButton 		ratePush	("RATE-PUSH", 	MCP23017::PORT_A,	MCP23017::P0, &arpSyncPush, 	&rateEncoder);

PushButton 		add2Push	("ADD2-PUSH", 	MCP23017::PORT_B,	MCP23017::P5, NULL, 		&add2Encoder);
PushButton 		add1Push	("ADD1-PUSH", 	MCP23017::PORT_B,	MCP23017::P4, &add2Push,	&add1Encoder);

MCP23017 mcp101(&hi2c1, MCP23017::ADDR_101, &ratePush, &arpSyncEncoder, &add1Push, &add2Encoder);

// ===== north board MCP_111 (EXTI_4 / EXTI_5) =====

RotaryEncoder 	latchEncoder	("LATCH-ENC", 	MCP23017::PORT_A,	MCP23017::P5, MCP23017::P4, NULL);
RotaryEncoder 	reverbEncoder	("REV-ENC", 	MCP23017::PORT_A,	MCP23017::P1, MCP23017::P2, &latchEncoder);

RotaryEncoder 	portaEncoder	("PORT-ENC", 	MCP23017::PORT_B,	MCP23017::P7, MCP23017::P6, NULL);
RotaryEncoder 	onoffEncoder	("ONOFF-ENC", 	MCP23017::PORT_B,	MCP23017::P4, MCP23017::P3, &portaEncoder);
RotaryEncoder 	backfEncoder	("BF-ENC", 		MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &onoffEncoder);

PushButton 		backfPush	("BACKF-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, NULL, 		&backfEncoder);
PushButton 		latchPush	("LATCH-PUSH", 	MCP23017::PORT_A,	MCP23017::P6, &backfPush, 	&latchEncoder);
PushButton 		delayPush	("DELAY-PUSH", 	MCP23017::PORT_A,	MCP23017::P3, &latchPush, 	&delayEncoder);
PushButton 		reverbPush	("REVERB-PUSH", MCP23017::PORT_A,	MCP23017::P0, &delayPush, 	&reverbEncoder);

PushButton 		portaPush	("PORTA-PUSH", 	MCP23017::PORT_B,	MCP23017::P5, NULL, 		&portaEncoder);
PushButton 		onoffPush	("ONOFF-PUSH", 	MCP23017::PORT_B,	MCP23017::P2, &portaPush, 	&onoffEncoder);

MCP23017 mcp111(&hi2c1, MCP23017::ADDR_111, &reverbPush, &reverbEncoder, &onoffPush, &backfEncoder);

#ifdef SOUTH_BOARD_CONNECTED
// ==== south board, western part, address 000 (EXTI_12 & EXTI_11) ====

RotaryEncoder 	tun1Encoder		("TUNE1-ENC", 	MCP23017::PORT_A,	MCP23017::P4, MCP23017::P3, NULL);
RotaryEncoder 	mix1Encoder		("MIX1-ENC", 	MCP23017::PORT_A,	MCP23017::P0, MCP23017::P1, &tun1Encoder);

RotaryEncoder 	mixDigEncoder	("MIXD-ENC", 	MCP23017::PORT_B,	MCP23017::P7, MCP23017::P6, NULL);
RotaryEncoder 	tunDigEncoder	("TUNED-ENC", 	MCP23017::PORT_B,	MCP23017::P3, MCP23017::P2, &mixDigEncoder);
RotaryEncoder 	wavDigEncoder	("WAVED-ENC", 	MCP23017::PORT_B,	MCP23017::P1, MCP23017::P0, &tunDigEncoder);

PushButton		octDigPush		("OCTD-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, NULL, NULL);
PushButton		wavDigPush		("WAVED-PUSH", 	MCP23017::PORT_A,	MCP23017::P6, &octDigPush, &wavDigEncoder);
PushButton		tun1Push		("TUNE1-PUSH", 	MCP23017::PORT_A,	MCP23017::P5, &wavDigPush, &tun1Encoder);
PushButton		mix1Push		("MIXE1-PUSH", 	MCP23017::PORT_A,	MCP23017::P2, &tun1Push, &mix1Encoder);

PushButton		mixDigPush		("MIXD-PUSH", 	MCP23017::PORT_B,	MCP23017::P5, NULL, &mixDigEncoder);
PushButton		tunDigPush		("TUNED-PUSH", 	MCP23017::PORT_B,	MCP23017::P4, &mixDigPush, &tunDigEncoder);

MCP23017 mcp000(&hi2c3, MCP23017::ADDR_000, &mix1Push, &mix1Encoder, &tunDigPush, &wavDigEncoder);

// ==== south board, western part, address 001 (EXTI_9 & EXTI_10) ====

RotaryEncoder 	tune2Encoder	("TUNE2-ENC", 	MCP23017::PORT_A,	MCP23017::P3, MCP23017::P4, NULL);
RotaryEncoder 	mix2Encoder		("MIX2-ENC", 	MCP23017::PORT_A,	MCP23017::P0, MCP23017::P2, &tune2Encoder);

RotaryEncoder 	wave1Encoder	("WAVE1-ENC", 	MCP23017::PORT_B,	MCP23017::P6, MCP23017::P5, NULL);
RotaryEncoder 	wave2Encoder	("WAVE2-ENC", 	MCP23017::PORT_B,	MCP23017::P1, MCP23017::P3, &wave1Encoder);

PushButton		syncVcoPush		("SYNCVCO-PUSH", MCP23017::PORT_A,	MCP23017::P7, NULL, NULL);
PushButton		oct2Push		("OCT2-PUSH", 	MCP23017::PORT_A,	MCP23017::P6, &syncVcoPush, NULL);
PushButton		tune2Push		("TUNE2-PUSH", 	MCP23017::PORT_A,	MCP23017::P5, &oct2Push, &tune2Encoder);
PushButton		mix2Push		("MIX2-PUSH", 	MCP23017::PORT_A,	MCP23017::P1, &tune2Push, &mix2Encoder);

PushButton		oct1Push		("OCT1-PUSH", 	MCP23017::PORT_B,	MCP23017::P7, NULL, NULL);
PushButton		wave1Push		("WAVE1-PUSH", 	MCP23017::PORT_B,	MCP23017::P4, &oct1Push, &wave1Encoder);
PushButton		wave2Push		("WAVE2-PUSH", 	MCP23017::PORT_B,	MCP23017::P2, &wave1Push, &wave2Encoder);

MCP23017 mcp001(&hi2c3, MCP23017::ADDR_001, &mix2Push, &mix2Encoder, &wave2Push, &wave2Encoder);

// ==== south board, eastern part, address 010 ====


RotaryEncoder 	lfo2AmountEncoder	("LFO2AMNT-ENC", 	MCP23017::PORT_A,	MCP23017::P7, MCP23017::P5, NULL);
RotaryEncoder 	lfo2RateEncoder		("LFO2RATE-ENC", 	MCP23017::PORT_A,	MCP23017::P2, MCP23017::P1, &lfo2AmountEncoder);

RotaryEncoder 	lfo1RateEncoder		("LFO1RATE-ENC", 	MCP23017::PORT_B,	MCP23017::P5, MCP23017::P4, NULL);
RotaryEncoder 	lfo1AmountEncoder	("LFO1AMNT-ENC", 	MCP23017::PORT_B,	MCP23017::P2, MCP23017::P1, &lfo1RateEncoder);

PushButton		lfo2AmountPush		("LFO2AMNT-PUSH", 	MCP23017::PORT_A,	MCP23017::P6, NULL, &lfo2AmountEncoder);
PushButton		lfo2XXXPush			("LFO2XXX-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &lfo2AmountPush, NULL);
PushButton		lfo2RatePush		("LFO2RATE-PUSH", 	MCP23017::PORT_A,	MCP23017::P3, &lfo2XXXPush, &lfo2RateEncoder);
PushButton		lfo2SyncPush		("LFO2SYNC-PUSH", 	MCP23017::PORT_A,	MCP23017::P0, &lfo2RatePush, NULL);

PushButton		lfo1SyncPush		("LFO1SYNC-PUSH", 	MCP23017::PORT_B,	MCP23017::P7, NULL, NULL);
PushButton		lfo1RatePush		("LFO1RATE-PUSH", 	MCP23017::PORT_B,	MCP23017::P6, &lfo1SyncPush, &lfo1RateEncoder);
PushButton		lfo1AmountPush		("LFO1AMNT-PUSH", 	MCP23017::PORT_B,	MCP23017::P3, &lfo1RatePush, &lfo1AmountEncoder);
PushButton		lfo1XXXPush			("LFO1XXX-PUSH", 	MCP23017::PORT_B,	MCP23017::P0, &lfo1AmountPush, NULL);

MCP23017 mcp010(&hi2c2, MCP23017::ADDR_010, &lfo2SyncPush, &lfo2RateEncoder, &lfo1XXXPush, &lfo1AmountEncoder);

// ==== south board, eastern part, address 011 ====

RotaryEncoder 	egAmountEncoder		("EGAMNT-ENC", 		MCP23017::PORT_A,	MCP23017::P6, MCP23017::P5, NULL);
RotaryEncoder 	cutoffEncoder		("CUTOFF-ENC", 		MCP23017::PORT_A,	MCP23017::P1, MCP23017::P0, &egAmountEncoder);

RotaryEncoder 	vcfDEncoder			("VCFD-ENC", 		MCP23017::PORT_B,	MCP23017::P6, MCP23017::P5, NULL);
RotaryEncoder 	vcfSEncoder			("VCFS-ENC", 		MCP23017::PORT_B,	MCP23017::P3, MCP23017::P4, &vcfDEncoder); // marche pas
RotaryEncoder 	vcfREncoder			("VCFR-ENC", 		MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &vcfSEncoder);

PushButton		vcfRPush			("VCFR-PUSH", 		MCP23017::PORT_A,	MCP23017::P7, NULL, &vcfREncoder);
PushButton		egAmountPush		("EGAMNT-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &vcfRPush, &egAmountEncoder);
PushButton		orderPush			("ORDER-PUSH", 		MCP23017::PORT_A,	MCP23017::P3, &egAmountPush, NULL);
PushButton		cutoffPush			("CUTOFF-PUSH", 	MCP23017::PORT_A,	MCP23017::P2, &orderPush, &cutoffEncoder);

PushButton		vcfDPush			("VCFD-PUSH", 		MCP23017::PORT_B,	MCP23017::P7, NULL, &vcfDEncoder);
PushButton		vcfSPush			("VCFS-PUSH", 		MCP23017::PORT_B,	MCP23017::P2, &vcfDPush, &vcfSEncoder);

MCP23017 mcp011(&hi2c2, MCP23017::ADDR_011, &cutoffPush, &cutoffEncoder, &vcfSPush, &vcfREncoder);


// ==== south board, eastern part, address 100 ====

RotaryEncoder 	vcaSEncoder		("VCAS-ENC", 	MCP23017::PORT_A,	MCP23017::P5, MCP23017::P6, NULL);
RotaryEncoder 	vcaREncoder		("VCAR-ENC", 	MCP23017::PORT_A,	MCP23017::P2, MCP23017::P3, &vcaSEncoder);

RotaryEncoder 	vcfAEncoder		("VCFA-ENC", 	MCP23017::PORT_B,	MCP23017::P6, MCP23017::P5, NULL);
RotaryEncoder 	vcaAEncoder		("VCAA-ENC", 	MCP23017::PORT_B,	MCP23017::P2, MCP23017::P3, &vcfAEncoder);
RotaryEncoder 	vcaDEncoder		("VCAD-ENC", 	MCP23017::PORT_B,	MCP23017::P0, MCP23017::P1, &vcaAEncoder);

PushButton		vcaDPush		("VCAD-PUSH", 	MCP23017::PORT_A,	MCP23017::P7, NULL, &vcaDEncoder);
PushButton		vcaSPush		("VCAS-PUSH", 	MCP23017::PORT_A,	MCP23017::P4, &vcaDPush, &vcaSEncoder);
PushButton		vcaRPush		("VCAR-PUSH", 	MCP23017::PORT_A,	MCP23017::P1, &vcaSPush, &vcaREncoder);
PushButton		eg3Push			("EG3-PUSH", 	MCP23017::PORT_A,	MCP23017::P0, &vcaRPush, NULL);

PushButton		vcfAPush		("VCFA-PUSH", 	MCP23017::PORT_B,	MCP23017::P7, NULL, &vcfAEncoder);
PushButton		vcaAPush		("VCAA-PUSH", 	MCP23017::PORT_B,	MCP23017::P4, &vcfAPush, &vcaAEncoder);

MCP23017 mcp100(&hi2c2, MCP23017::ADDR_100, &eg3Push,  &vcaREncoder, &vcaAPush, &vcaDEncoder);

// list of all MCP devices (shall be used to check for events):
MCP23017* mcps[8]={&mcp000, &mcp001, &mcp010, &mcp011, &mcp100, &mcp101, &mcp110, &mcp111};
#else
MCP23017* mcps[3]={&mcp101, &mcp110, &mcp111};
#endif

const int mcpCount = sizeof(mcps)/sizeof(*mcps);


// list of all Neopixel daisychains:
TLC59731 tlcNorth(&hspi2);
#ifdef SOUTH_BOARD_CONNECTED
TLC59731 tlcSouth1(&hspi1);
TLC59731 tlcSouth2(&hspi3);
#endif

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

#ifdef SOUTH_BOARD_CONNECTED
	mcp000.printAttachedControllers();
	mcp001.printAttachedControllers();
	mcp010.printAttachedControllers();
	mcp011.printAttachedControllers();
	mcp100.printAttachedControllers();
#endif

	tlcNorth.test();

#ifdef SOUTH_BOARD_CONNECTED
	tlcSouth1.test();
	tlcSouth2.test();
#endif
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
// * ~3ms for the whole loop when one button pressed or rotated (for 3 MCP's)
static void checkEvent(){

	//tx2.on();
	for (int i=0; i<mcpCount; i++){
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

		/*intensity += 100;
		intensity %= 200;
		for (int led=0; led < TLC_LED_COUNT; led++)  {
			tlcNorth.update(led, 0, 0, intensity);
		}
		tlcNorth.transmitData();
		for (int led=0; led < TLC_LED_COUNT; led++)  {
			tlcSouth1.update(led, 0, intensity, 0);
		}
		tlcSouth1.transmitData();
		for (int led=0; led < TLC_LED_COUNT; led++)  {
			tlcSouth2.update(led, intensity, 0, 0);
		}
		tlcSouth2.transmitData();*/

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
		HAL_Delay(100);
	}
}

