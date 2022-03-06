/*
 * TLC59731.cpp
 *
 *  Created on: Nov 18, 2020
 *      Author: sydxrey
 *
 * Timings: 1 bit neopixel = 8 bits SPI (MOSI) = 1 byte of SPI
 *
 * <---------------> tCycle = 8 x Tspi
 *  _
 * | |______________ = 0 (neopixel) = iooooooo = 0x80 sur le bus SPI
 *  _    _
 * | |__| |_________ = 1 (neopixel) = ioiooooo = 0xA0 sur le bus SPI
 *
 *  Steps :
 *  Write 0x3A (neo) = 0011 1010 (neo) ; first two bits help measure tCycle
 *  then write 24 bits of data (=3 x 8 bits per color)
 *  then write 4tCycle of zero = 0x0000 (SPI) this is the EOS, and following bits are sent to next neopixel
 *  or
 *  write 8tCycle of zero = 0x00000000 (SPI), this is the GSLAT sequence, which copies data to the PWM controllers
 *
 */

#include "TLC59731.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdlib.h>

//extern SPI_HandleTypeDef hspi1;

TLC59731::TLC59731(SPI_HandleTypeDef *hspi) {

	_hspi = hspi;

	for (int i = 0; i < TLC_BUF_SZ; i++)
		_spiBuf[i] = 0;

	// init SPI buffer for all the LEDs :
	for (int led=0; led < TLC_LED_COUNT; led++) initSpiBuffer(led);

}

TLC59731::~TLC59731() {

}

/**
 * Init the (previously allocated) SPI buffer starting at the position corresponding to the given led
 */
void TLC59731::initSpiBuffer(uint8_t led) {

	int i = getDataAreaIndex(led);

	_spiBuf[i++] = TLC_ZERO; // write command 0x3A
	_spiBuf[i++] = TLC_ZERO;
	_spiBuf[i++] = TLC_ONE;
	_spiBuf[i++] = TLC_ONE;
	_spiBuf[i++] = TLC_ONE;
	_spiBuf[i++] = TLC_ZERO;
	_spiBuf[i++] = TLC_ONE;
	_spiBuf[i++] = TLC_ZERO;
	// RGB data at 0% for each color:
	for (int j = 0; j < 24; j++)
		_spiBuf[i++] = TLC_ZERO;
}

void TLC59731::test() {

	//RGBColor colors[] = {RGBColor::YELLOW, RGBColor::TURQUOISE, RGBColor::RED, RGBColor::BLUE, RGBColor::CYAN, RGBColor::GREEN, RGBColor::WHITE};
	//RGBColor colors[] = {RGBColor::RED, RGBColor::BLUE, RGBColor::GREEN, RGBColor::WHITE};
	RGBColor col = RGBColor::RED;

	//int i = 0;
	while (1) {  // BRG sauf LED => RBG

			//for (int led=0; led < TLC_LED_COUNT; led++)  update(led, colors[(i++)%7]);
			//for (int led=0; led < TLC_LED_COUNT; led++)  update(led, 20, 20, 5 * led);
			for (int led=0; led < TLC_LED_COUNT; led++)  {
				col.randomize(50);
				update(led, col);
			}
			//i++;


			/*
			// modulation: B R G
			update(i++, 0, 0, 255); transmitData();   // B R G
			update(i++, 100, 0, 200); transmitData(); // R B G
			update(i++, 50, 0, 250); transmitData(); // B R G
			update(i++, 50, 150, 200); transmitData(); // R B G
			// dig osc
			update(i++, 20, 100, 0); transmitData(); // B R G
			update(i++, 20, 180, 20); transmitData(); // B R G
			update(i++, 250, 00, 100); transmitData(); // R B G
			update(i++, 0, 255, 100); transmitData(); // B R G
			// analog osc 1
			update(i++, 255, 0, 0); transmitData(); // B R G
			update(i++, 50, 200, 50); transmitData(); // R B G  LED
			update(i++, 180, 20, 100); transmitData(); // B R G
			update(i++, 250, 50, 80); transmitData(); // B R G
			// analog osc 2
			update(i++, 100, 100, 0); transmitData();
			update(i++, 120, 80, 0); transmitData();
			update(i++, 150, 160, 0); transmitData(); // LED : R B G
			update(i++, 200, 200, 0); transmitData(); // LED : R B G
			update(i++, 200, 100, 200); transmitData();
			 */

			transmitData();
			HAL_Delay(500);

	}

	/*while (1) {
			//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			HAL_Delay(500);
			//i++;
			//if (i>6) i=0;

	}*/

}

/**
 * update the area of the SPI buffer containing RGB data for the given LED.
 * Leaves other areas unchanged, in particular the Write Command header and the EOS area.
 */
void TLC59731::update(uint8_t led, uint8_t red256, uint8_t green256, uint8_t blue256) {

	//uint8_t green256 = (uint8_t) (255 * green) & 0xFF;
	//uint8_t blue256 = (uint8_t) (255 * blue) & 0xFF;
	//uint8_t red256 = (uint8_t) (255 * red) & 0xFF;

	int i = getDataAreaIndex(led) + 8; // beginning of RGB data for the given led

	uint8_t mask = 0x80;

	// data led #1 : R, G, B (8 bits each)
	//mask = 0x80; // = 1000 0000
	for (int j = 0; j < 8; j++) {
		_spiBuf[i++] = (mask & green256) == 0 ? TLC_ZERO : TLC_ONE;
		mask >>= 1; // take next bit
	}
	mask = 0x80;
	for (int j = 0; j < 8; j++) {
		_spiBuf[i++] = (mask & blue256) == 0 ? TLC_ZERO : TLC_ONE;
		mask >>= 1; // take next bit
	}
	mask = 0x80;
	for (int j = 0; j < 8; j++) {
		_spiBuf[i++] = (mask & red256) == 0 ? TLC_ZERO : TLC_ONE;
		mask >>= 1; // take next bit
	}

}

void TLC59731::update(uint8_t led, uint32_t rgb24bits) {

	//uint8_t green256 = (uint8_t) (255 * green) & 0xFF;
	//uint8_t blue256 = (uint8_t) (255 * blue) & 0xFF;
	//uint8_t red256 = (uint8_t) (255 * red) & 0xFF;

	int i = getDataAreaIndex(led) + 8; // beginning of RGB data for the given led

	uint32_t mask = 0x800000;

	// data led #1 : R, G, B (8 bits each)
	for (int j = 0; j < 24; j++) {
		_spiBuf[i++] = (mask & rgb24bits) == 0 ? TLC_ZERO : TLC_ONE;
		mask >>= 1; // take next bit
	}

}

void TLC59731::update(uint8_t led, RGBColor& color) {

	update(led, color.m_red, color.m_blue, color.m_green);

}

void TLC59731::transmitData() {
	HAL_SPI_Transmit(_hspi, _spiBuf, TLC_BUF_SZ, 100);
}

int TLC59731::getDataAreaIndex(uint8_t led) {

	return led * (32 + TLC_EOS_SZ); // 8 data bits for the write command + 24 RGB data + "TLC_EOS_SZ" EOS bits
}
