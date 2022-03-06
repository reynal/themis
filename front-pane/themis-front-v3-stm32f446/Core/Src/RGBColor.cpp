/*
 * Color.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: sydxrey
 */

#include <RGBColor.h>
#include "stdlib.h"

RGBColor::RGBColor() {
	m_red = 0;
	m_green = 0;
	m_blue = 0;

}

RGBColor::RGBColor(uint8_t red, uint8_t green, uint8_t blue){
	m_red = red;
	m_green = green;
	m_blue = blue;
}

void RGBColor::randomize(uint8_t intensity){
	 m_red = (uint8_t)(intensity*rand());
	 m_green = (uint8_t)(intensity*rand());
	 m_blue = (uint8_t)(intensity*rand());
}

void RGBColor::fade(double alpha){

	m_red = (uint8_t) (alpha * m_red);
	m_green = (uint8_t) (alpha * m_green);
	m_blue = (uint8_t) (alpha * m_blue);

}

void RGBColor::interpolate(RGBColor& c1, RGBColor& c2, double alpha){

	m_red = (1-alpha) * c1.m_red + alpha * c2.m_red;
	m_green = (1-alpha) * c1.m_green + alpha * c2.m_green;
	m_blue = (1-alpha) * c1.m_blue + alpha * c2.m_blue;
}

void RGBColor::set(RGBColor& c){
	m_red = c.m_red;
	m_green = c.m_green;
	m_blue = c.m_blue;
}



const RGBColor RGBColor::RED(		0xFF, 0x00,0x00);
const RGBColor RGBColor::GREEN(		0x00, 0xFF, 0x00);
const RGBColor RGBColor::BLUE(		0x00,0x00, 0xFF);
const RGBColor RGBColor::WHITE(		0xFF, 0xFF, 0xFF);
const RGBColor RGBColor::BLACK(		0x00,0x00,0x00);
const RGBColor RGBColor::CYAN(		0x00, 0xFF, 0xFF);
const RGBColor RGBColor::YELLOW(	0xFF, 0xFF, 0x00);
const RGBColor RGBColor::MAGENTA(	0xFF, 0x00, 0xFF);
const RGBColor RGBColor::ORANGE(	0xFF, 0xA5, 0x00);
const RGBColor RGBColor::TURQUOISE(	0x40, 0xE0, 0xD0);


/*RGBColor HsvToRgb(HsvColor hsv){

    RgbColor rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

HsvColor RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}*/
