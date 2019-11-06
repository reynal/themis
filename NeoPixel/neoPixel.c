/*
 * neoPixel.c
 *
 *  Created on: Nov 5, 2019
 *      Author: maxime
 *
 *  This contain the functions used to control NeoPixels.
 *  A NeoPixel device is represented by a neopixel struct.
 *
 *  To change the color of the NeoPixels we comunicate with them throught SPI, we send 100 to send a 0 and 110 to send a 1.
 */


#include <neoPixel.h>
/*
 * Initialise a neopixel struct representing a NeoPixel device
 * Arguments :
 * 		ret : pointer to the struct we want to initialise
 * 		npixel : number of pixels on the device
 */
void nP_create(neopixel* ret,uint32_t npixel){ //Initialise la structure
	ret->npixel = npixel;
	uint8_t red[npixel];
	uint8_t green[npixel];
	uint8_t blue[npixel];
	uint8_t bufferSPI[3 * 3 * 8 * npixel]; //we send 3 bits to give a bit of data to neopixels, each neopixels have 3 colors, each stored on 8 bits.
	ret->red = red;
	ret->green = green;
	ret->blue = blue;
	ret->bufferSPI = bufferSPI;
	;
}

/*
 * Chose the color of a pixel of a NeoPixel device
 * Arguments:
 * 		np : struct representing the NeoPixel device
 * 		n : wich pixel we want to color, starts at 0
 * 		rgb : RGB code of the color
 */
void nP_setPixel(neopixel* np,uint32_t n, uint32_t rgb){ //permet de donner une valeur rgb à 1 pixel
	if(n < np->npixel){
		np->red[n] = 0xFF0000 & rgb;
		np->green[n] = 0x00FF00 & rgb;
		np->blue[n] = 0x0000FF & rgb;
	}
}
/*
void nP_prepareMessage(neopixel* np){
	for(int i=0;i < np->npixel;i++){

	}
}

uint8*/

