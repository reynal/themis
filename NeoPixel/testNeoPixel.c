#include "testNeoPixel.h"

//Test if the SPI works
void tnP_testSPI(){
	uint8_t mess[27] = "La jolie baie des tilleuls";

	SPI_HandleTypeDef SpiHandle;
	SpiHandle.Instance = SPInp;
	while(1)
		HAL_SPI_Transmit(&SpiHandle, mess, 27, 1000);
}

//Light up a led
void tnP_test1(){
	neopixel np;
	SPI_HandleTypeDef SpiHandle;
	SpiHandle.Instance = SPInp;
	nP_create(&np,1);
	nP_setPixel(&np, 0, BLANC );
	nP_send(&np, SpiHandle);
}
