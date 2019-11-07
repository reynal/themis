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
 *  For each pixel we need 1 byte for each of the RGB colors so a total of 72 bits will be transported throught SPI
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
 * Update the content of bufferSPI of a neopixel struct to match its RGB values
 * Arguments:
 * 		np : the neopixel struct which need an update
 */
void nP_prepareMessage(neopixel* np){
	int index = 0;
	for(int i=0;i < np->npixel;i++){
		uint32_t greenColor = nP_convertByteSPI(np->green[i]);
		nP_concat(np->bufferSPI,&index,greenColor);
		uint32_t redColor = nP_convertByteSPI(np->red[i]);
		nP_concat(np->bufferSPI,&index,redColor);
		uint32_t blueColor = nP_convertByteSPI(np->blue[i]);
		nP_concat(np->bufferSPI,&index,blueColor);
	}
}

/*
 * Convert a byte representing a color to a 24 bits word which will be send throught SPI to control a led
 * Argument:
 * 		color : the byte representing a component of a color
 * 	Output:
 * 		An int whose 24 first bits are converted to be send to the NeoPixel device throught SPI
 * 	Example:
 * 		if we give 10010100 as an input the outpu will be 110 100 100 110 100 110 100 100
 */
uint32_t nP_convertByteSPI(uint8_t color){
	uint32_t ret = 0;
	for(int i=7; i>=0; i--){ //We must send the MSB first
		ret = ret >> 3;
		if((color << i) & 1) //We look at the i-th bit of color
			ret += 6; //110
		else
			ret += 4; //100
	}
	return ret;
}

/*
 * Put the result of nP_convertByteSPI at the end of the buffer to send
 * Arguments:
 * 		bufferSPI the buffer being filled
 * 		index : a dynamicaly evolving number tracking the sise of bufferSPI
 * 		color : the 24 bits word which will be placed at the end of bufferSPI
 */
void nP_concat(uint8_t* bufferSPI,int* index,uint32_t color){
	for(int i=0; i<3 ;i++){ //there is 3 times 8 bits in the word color
		bufferSPI[*index] = (char) (color << (8 * i)) & 0xFF;
		*index += 1;
	}
}


