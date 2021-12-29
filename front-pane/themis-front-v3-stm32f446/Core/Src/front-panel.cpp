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

// ------------------------------ defines -----------------------------------

/*#define COLOR_RED		0xFF0000
#define COLOR_ORANGE	0xFF7F00
#define COLOR_YELLOW	0xFFFF00
#define COLOR_GREEN		0x00FF00
#define COLOR_BLUE		0x0000FF
#define COLOR_INDIGO 	0x2E2B5F
#define	COLOR_VIOLET	0x8B00FF*/

#define MIDI_CC			0xB0    // 176

// ------------------------------ variables -----------------------------------

//uint32_t colors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, COLOR_INDIGO, COLOR_VIOLET};
RGBColor colors1[] = {RGBColor::YELLOW, RGBColor::TURQUOISE, RGBColor::BLUE};
RGBColor colors2[] = {RGBColor::RED, RGBColor::ORANGE, RGBColor::CYAN};
int color_count = sizeof(colors1)/sizeof(*colors1);

uint8_t cc0[] = {20,21,22};
uint8_t cc1[] = {23,24,25};

extern char print_buffer[50];
extern I2C_HandleTypeDef hi2c1;

MCP23017 MCP23017_array[1];
/*RotaryEncoder RotaryEncoder_array[] = {
		RotaryEncoder("rot1", MCP23017::P1, MCP23017::P0),
		RotaryEncoder("rot2", MCP23017::P3, MCP23017::P2)
};
int RotaryEncoder_count = sizeof(RotaryEncoder_array)/sizeof(*RotaryEncoder_array);
*/

PushButton PushButtonA_array[] = {
		PushButton("pushA0", MCP23017::P0),
		PushButton("pushA1", MCP23017::P1),
		PushButton("pushA2", MCP23017::P2),
		PushButton("pushA3", MCP23017::P3),
		PushButton("pushA4", MCP23017::P4),
		PushButton("pushA5", MCP23017::P5),
		PushButton("pushA6", MCP23017::P6),
		PushButton("pushA7", MCP23017::P7)
};
int PushButtonA_count = sizeof(PushButtonA_array)/sizeof(*PushButtonA_array);

PushButton PushButtonB_array[] = {
		PushButton("pushB0", MCP23017::P0),
		PushButton("pushB1", MCP23017::P1),
		PushButton("pushB2", MCP23017::P2),
		PushButton("pushB3", MCP23017::P3),
		PushButton("pushB4", MCP23017::P4),
		PushButton("pushB5", MCP23017::P5),
		PushButton("pushB6", MCP23017::P6),
		PushButton("pushB7", MCP23017::P7)
};
int PushButtonB_count = sizeof(PushButtonB_array)/sizeof(*PushButtonB_array);

//TLC59731 tlc;

// ------------------------------ functions -----------------------------------

void init_hardware(){

	MCP23017_array[0].init(&hi2c1, MCP23017::ADDR_004);
	//tlc.transmitData();
}

void main_loop(){

	/*RGBColor rgb;
	int t=0;
	int colorIdx0=0;
	int colorIdx1=0;*/
	while(1){
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


		sprintf(print_buffer, "GPIOA=%d \t GPIOB=%d\n", MCP23017_array[0].read(MCP23017::GPIO_A), MCP23017_array[0].read(MCP23017::GPIO_B) ); printSerial();

		HAL_Delay(300);
	}

}

// ------------------------------ HAL Callback -----------------------------------


extern "C" {

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	MCP23017 *mcp = NULL;

	if (GPIO_Pin == MCP_111_INTA_Pin) { // DEBUG : TODO !
		mcp = MCP23017_array+0;
		uint8_t flagReg = mcp->read(MCP23017::INTFA); // 50us
		uint8_t capReg = mcp->read(MCP23017::INTCAPA);

		//sprintf(print_buffer, "INTCAPA=%d\n", capReg ); printSerial();


		/*for (int i=0; i<RotaryEncoder_count; i++){
			if ((flagReg & (RotaryEncoder_array+i)->mask) != 0){ // this encoder triggered the interrupt
				(RotaryEncoder_array+i)->updatePrevNextCode(capReg);
			}
		}*/

		for (int i=0; i<PushButtonA_count; i++){
			if ((flagReg & (PushButtonA_array+i)->pin) != 0){ // this button triggered the interrupt
				PushButton::State s = (capReg & (PushButtonA_array+i)->pin) == 0 ? PushButton::RELEASED : PushButton::PUSHED;
				(PushButtonA_array+i)->stateChanged(s);
			}
		}
	}
	else if (GPIO_Pin == MCP_111_INTB_Pin) { // DEBUG : TODO !
		mcp = MCP23017_array+0;
		uint8_t flagReg = mcp->read(MCP23017::INTFB); // 50us
		uint8_t capReg = mcp->read(MCP23017::INTCAPB);

		/*for (int i=0; i<RotaryEncoder_count; i++){
			if ((flagReg & (RotaryEncoder_array+i)->mask) != 0){ // this encoder triggered the interrupt
				(RotaryEncoder_array+i)->updatePrevNextCode(capReg);
			}
		}*/

		for (int i=0; i<PushButtonB_count; i++){
			if ((flagReg & (PushButtonB_array+i)->pin) != 0){ // this button triggered the interrupt
				PushButton::State s = (capReg & (PushButtonB_array+i)->pin) == 0 ? PushButton::RELEASED : PushButton::PUSHED;
				(PushButtonB_array+i)->stateChanged(s);
			}
		}
	}

}

} // extern "C"


