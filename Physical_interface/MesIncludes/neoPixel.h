#ifndef NEOPIXEL
#define NEOPIXEL

#include <stdint.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

//SPI control
#define SPInp SPI1; //This one is correctely configured with TEstNeoLed.iox




/*
 * This struct is used to store the number of neopixels on the object we use and what color they should be
 */
typedef struct neopixel_s {
	uint32_t npixel; //number of pixels
	uint8_t* red; //red composant of each pixel
	uint8_t* green;
	uint8_t* blue;
	uint8_t* bufferSPI; //The message we send throught SPI to color the pixels
} neopixel;

//controling neopixel objects
void nP_create(neopixel* ret,uint32_t npixel);
void nP_setPixel(neopixel* np,uint32_t n, uint32_t rgb);

//sending the data
void nP_prepareMessage(neopixel* np);
uint32_t nP_convertByteSPI(uint8_t color);
void nP_concat(uint8_t* bufferSPI,int index,uint32_t color);
void nP_send(neopixel* np,SPI_HandleTypeDef SpiHandle);
void nP_sendDataGPIO(uint8_t* buffer,uint32_t nPixel);
void nP_sendByteGPIO(uint8_t data);
void nP_sendBitGPIO(uint8_t data);

#endif
