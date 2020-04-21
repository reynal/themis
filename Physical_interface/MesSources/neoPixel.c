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
	//uint8_t red[npixel];
	//uint8_t green[npixel];
	//uint8_t blue[npixel];
	//uint8_t bufferSPI[3 * 3 * npixel]; //we send 3 bits to give a bit of data to neopixels, each neopixels have 3 colors, each stored on 8 bits.
	uint8_t* red       = malloc(npixel);
	uint8_t* green     = malloc(npixel);
	uint8_t* blue      = malloc(npixel);
	uint8_t* bufferSPI = malloc(3 * 3 * npixel); //we send 3 bits to give a bit of data to neopixels, each neopixels have 3 colors, each stored on 8 bits.
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
		nP_concat(np->bufferSPI,i * 3 + 0,blueColor);
		uint32_t redColor = nP_convertByteSPI(np->red[i]);
		nP_concat(np->bufferSPI,i * 3 + 1,redColor);
		uint32_t greenColor = nP_convertByteSPI(np->green[i]);
		nP_concat(np->bufferSPI,i * 3 + 2,greenColor);
	}
	nP_switchEdianRArray(np->bufferSPI, np->npixel * 9);
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
		curretPlace[i] = (char) (color >> ((2-i) * 8)) & 0xFF;
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
	//while(1){ //debug
		//HAL_SPI_Transmit(&SpiHandle, np->bufferSPI, (np->npixel) * 9, 1000);
		//HAL_SPI_Transmit(&SpiHandle, np->bufferSPI,(np->npixel) * 16, 1000);
		//HAL_SPI_Transmit(&SpiHandle, np->bufferSPI,(np->npixel) * 12, 1000);
		//nP_sendDataGPIO(np->bufferSPI, np->npixel); //HAL_Delay(190); //Play with GPIOB1 istead of SPI
		nP_sendMessageGPIO(np->bufferSPI, np->npixel);
		//HAL_SPI_Transmit(&SpiHandle,(uint8_t*) &null, 1, 1000);
		//nP_sendReset(&SpiHandle);
		//while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {} //We fait for the message to be send
		//HAL_Delay(10);}
}


void nP_sendReset(SPI_HandleTypeDef* SpiHandle){
	uint32_t null = 0;
	for(int i=0; i< 100 * 18; i++)
		HAL_SPI_Transmit(SpiHandle,(uint8_t*) &null, 1, 1000); //We send a signal as long as 100 leds to reach with a margin the 50 µs pause.
}

/*
 * Send the buffer over GPIO
 */
void nP_sendDataGPIO(uint8_t* buffer,uint32_t nPixel){
	for(int i=0; i<nPixel * 9; i++){
		nP_sendByteGPIO(buffer[i]);
	}
}

/*
 * Send a byte message to GPIOB_1
 */
void nP_sendBitWraperGPIO(uint8_t data){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
	for(int i=7; i>=0; i--){ //We take into account that we read from the MSB to the LSB in every byte.
		//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, (data >> i) & 1);
		nP_sendBitGPIO((data >> i) & 1);
		HAL_Delay(1); //TEST
		//for(int i=1 ; i<100; i++){} //Small wait
	}
}

/*
 * Send a bit message to GPIOB_1 but written in asm
 */
void nP_sendBitGPIO(uint8_t data){
	uint32_t gpiod_odr = 0x48000414;
	uint32_t mask = 0xFFFFFFD;
	__asm ( "      MOV r0, %[odrrr];" //We put GPIOB_ODR in R0
			"      LDR r1, [r0];" //We put the content of GPIOB_ODR in R1
			"      CMP %[input], 0;" //We check if input is true or false
			"      BEQ false;"
			"      ORR r1, 0x2;" //We force the second bit of R1 to 1 to make GPIOG_1 on
			"      STR r1, [r0];" //We put the new R1 back into GPIOB_ODRR
			"      B end;"
			"	   false:;"
			"      AND r1, %[MASK];" //We force the secod but to 0 to make it false
			"      STR r1, [r0];"
			"      end:;"
				: //no result
				: [input] "r" (data), [odrrr] "r" (gpiod_odr), [MASK] "r" (mask)
				: "r0", "r1" //Cobbeled registers
		    );
	return;
}

uint8_t nP_switchEdian(uint8_t ch){
	char ret=0;
	for(int i=0; i<8; i++){
		char maskCh = 1 << i;
		char tmp = (ch & maskCh) >> i;
		ret += tmp << (7-i);
	}
	return ret;
}

void nP_switchEdianRArray(uint8_t* a, uint32_t len){
	for(uint32_t i=0; i<len; i++)
		a[i] = nP_switchEdian(a[i]);
}

/*
 * Send a byte message to GPIOB_1 but written in asm
 */
void nP_sendByteGPIO(uint8_t data){
	uint32_t gpiod_odr = 0x48000414;
	__asm ( "      MOV r0, %[odrrr];" //We put GPIOB_ODR in R0
			"      LDR r1, [r0];" //We put the content of GPIOB_ODR in R1
			"      MOV R2, %[input];"
			"      MOV R3, 0;" //Conteur de boucle
			"      startLoop:;"
			"      CMP R3, 8;"
			"      BEQ endloop;"
			"      AND R4, R2, 1;"
			"      LSR R2, 1;"
			"      LSL R4, 1;"
			"      STR R4, [r0];"
			"      ADD R3, 1;"
			"      B startLoop;"
			"      endloop:;"
				: //no result
				: [input] "r" (data), [odrrr] "r" (gpiod_odr)
				: "r0", "r1" ,"r2", "r3", "r4"//Cobbeled registers
		    );
	return;
}

/*
 * Send a full message to GPIOB_1 but written in asm
 */
void nP_sendMessageGPIO(uint8_t* a, uint32_t len){
	uint32_t gpiod_odr = 0x48000414;
	__asm ( "MOV r0, %[odrrr];" //We put GPIOB_ODR in R0
			"MOV R5, %[input];" //We get a pointer to the message to send
			"MOV R1, %[len];" //Total length of the message
			"startSend:"
			"LDR R2, [R5];" //We get a char of the message to send
			"MOV R3, 0;" //Loop over each bit of the char in R2
			"startLoopMes:;"
			"CMP R3, 8;" //We check if we went over all bits
			"BEQ endloopMes;"
			"AND R4, R2, 1;" //We get the bit we want
			"LSR R2, 1;" //We shift the char to send in order to be able to get the next bit
			"LSL R4, 1;" //We shift the bit of R4 in order to control GIOPB_1
			"STR R4, [r0];" //We put the bit in GPIO_ODR in order to control it
			"ADD R3, 1;" //We increase the loop conter
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;" //We wait a bit
			"B startLoopMes;"
			"endloopMes:;"
			"ADD R5, 4;" //We go over the next byte
			"SUB R1, 1;" //We check if we read all of the message
			"CMP R1, 0;"
			"BNE startSend;"
			"MOV R1, 0;" //We put the GPIO back on 0
			"STR R1, [R0];"
				: //no result
				: [input] "r" (a), [odrrr] "r" (gpiod_odr), [len] "r" (len * 9)
				: "r0", "r1" ,"r2", "r3", "r4", "r5"//Cobbeled registers
		    );
	return;
}
