/*
 * neoPixel.c
 *
 *  Created on: Nov 5, 2019
 *      Author: maxime
 *
 *  This contain the functions used to control NeoPixels.
 *  A NeoPixel device is represented by a neopixel struct.
 *
 *  To change the color of the NeoPixels we comunicate with them throught SPI,
 *  we send 100 to send a 0 and 110 to send a 1.
 *  For each pixel we need 1 byte for each of the RGB colors
 *  so a total of 72 bits will be transported throught SPI
 *
 *  Since each bit of data send to the NeoPixel must be 1.25 µs long the SPI bus will need to be send a bit each 416 ns.
 *  It will need to be clocked at 2.4 MHz.
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
	uint8_t bufferSPI[3 * 3 * 8 *npixel]; //we send 3 bits to give a bit of data to neopixels, each neopixels have 3 colors, each stored on 8 bits.
	ret->red = red;
	ret->green = green;
	ret->blue = blue;
	ret->bufferSPI = bufferSPI;

    for(int i=0; i<3 * 3 * npixel; i++)
        bufferSPI[i] = 0;

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
		np->red[n] = (0xFF0000 & rgb) >> 16;
		np->green[n] = (0x00FF00 & rgb) >> 8;
		np->blue[n] = (0x0000FF & rgb) >> 0;
	}
}
/*
 * Update the content of bufferSPI of a neopixel struct to match its RGB values
 * Arguments:
 * 		np : the neopixel struct which need an update
 */
void nP_prepareMessage(neopixel* np){
	for(int i=0;i < np->npixel;i++){
		uint32_t blueColor = nP_convertByteSPI(np->blue[i]);
		nP_concat(np->bufferSPI,i * 3 + 2,blueColor);
		uint32_t redColor = nP_convertByteSPI(np->red[i]);
		nP_concat(np->bufferSPI,i * 3 + 1,redColor);
		uint32_t greenColor = nP_convertByteSPI(np->green[i]);
		nP_concat(np->bufferSPI,i * 3 + 0,greenColor);
	}
}

/*
 * Convert a byte representing a color to a 24 bits word which will be send throught SPI to control a led
 * Argument:
 * 		color : the byte representing a component of a color
 * 	Output:
 * 		An int whose 24 first bits are converted to be send to the NeoPixel device throught SPI
 * 	Example:
 * 		if we give 10010100 as an input the output will be 110 100 100 110 100 110 100 100
 */
uint32_t nP_convertByteSPI(uint8_t color){
	uint32_t ret = 0;
	for(int i=7; i>=0; i--){ //We must send the MSB first
		ret = ret << 3;
		if((color >> i) & 1) //We look at the i-th bit of color
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
 * 		index : a dynamicaly evolving number tracking the size of bufferSPI.
 * 		color : the 24 bits word which will be placed at the end of bufferSPI
 */
void nP_concat(uint8_t* bufferSPI,int index,uint32_t color){
	uint8_t* curretPlace = bufferSPI + index * 3;
	for(int i=0; i<3; i++){
		curretPlace[i] = (char) (color >> (i * 8)) & 0xFF;
	}
}

/*
 * Send the data to update the leds
 * Arguments:
 * 		np : the neopixel struct representing the leds to update
 *
 * Note regarding the SPI transimision :
 * 		For each neopixel we sent 3 color code.
 * 		Each color code represent 8 bits of data and are send over 24 bits throught SPI
 * 		For each pixel we send 8 times a 9 bit word (the packet size we use)
 */
void nP_send(neopixel* np, SPI_HandleTypeDef SpiHandle){
	//uint32_t null = 0;
	while(1){ //debug
		//HAL_SPI_Transmit(&SpiHandle, np->bufferSPI, (np->npixel) * 9, 1000);
		HAL_SPI_Transmit(&SpiHandle, np->bufferSPI,(np->npixel) * 16, 1000);
		//HAL_SPI_Transmit(&SpiHandle,(uint8_t*) &null, 1, 1000);
		//nP_sendReset(&SpiHandle);
		//while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {} //We fait for the message to be send
		HAL_Delay(10);}
}


void nP_sendReset(SPI_HandleTypeDef* SpiHandle){
	uint32_t null = 0;
	for(int i=0; i< 100 * 18; i++)
		HAL_SPI_Transmit(SpiHandle,(uint8_t*) &null, 1, 1000); //We send a signal as long as 100 leds to reach with a margin the 50 µs pause.
}


void nP_sendDataGPIO(uint8_t* buffer,uint32_t nPixel){
	for(int i=0; i<nPixel; i++){
		nP_sendByteGPIO(buffer[i]);
	}
}

void nP_sendByteGPIO(uint8_t data){
	for(int i=0; i<8; i++){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (data >> i) & 1);
		HAL_Delay(416);
	}
}
