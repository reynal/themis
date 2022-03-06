/*
 * Color.h
 *
 *  Created on: Nov 23, 2020
 *      Author: sydxrey
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "stm32f4xx_hal.h"

class RGBColor {
public:
	RGBColor();
	RGBColor(uint8_t red, uint8_t green, uint8_t blue);

	static const RGBColor RED;
	static const RGBColor GREEN;
	static const RGBColor BLUE;
	static const RGBColor WHITE;
	static const RGBColor BLACK;
	static const RGBColor CYAN;
	static const RGBColor YELLOW;
	static const RGBColor MAGENTA;
	static const RGBColor ORANGE;
	static const RGBColor TURQUOISE;

	void fade(double alpha);

	void interpolate(RGBColor& c1, RGBColor& c2, double alpha);

	void set(RGBColor& c);

	void randomize(uint8_t intensity);

    uint8_t m_red;
    uint8_t m_green;
    uint8_t m_blue;
};



#endif /* COLOR_H_ */
