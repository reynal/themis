#include "testNeoPixel.h"

void tnP_testGPIO(){
	//uint8_t mess[27] = "La jolie baie des tilleuls";
	uint8_t mess[4] = {0x00,0xFF,0x11,0x00};

	while(1)
		//HAL_SPI_Transmit(&SpiHandle, mess, 27, 1000);
		nP_sendDataGPIO(mess, 4);
}

//Test if the SPI works
//Result : We see the message on the A6 pin
void tnP_testSPI(){
	//uint8_t mess[27] = "La jolie baie des tilleuls";
	uint8_t mess[9] = {0x00,0xFF,0x11,0x00,0x00,0x00,0x00,0x00,0x00};

	SPI_HandleTypeDef SpiHandle;
	SpiHandle.Instance = SPInp;
	while(1)
		//HAL_SPI_Transmit(&SpiHandle, mess, 27, 1000);
		HAL_SPI_Transmit(&SpiHandle, mess, 9, 1);
}

void tnP_testLed0(){
	uint8_t mess[9] = {0xDB,0xB6,0x6D,0x49,0x92,0x24,0x49,0x92,0x24};

	SPI_HandleTypeDef SpiHandle;
	SpiHandle.Instance = SPInp;
	while(1){
		HAL_SPI_Transmit(&SpiHandle, mess, 9, 1);
		HAL_Delay(1);
	}
}

//Light up a led
void tnP_test1(){
	neopixel np;
	SPI_HandleTypeDef SpiHandle;
	SpiHandle.Instance = SPInp;
	nP_create(&np,1);
	nP_setPixel(&np, 0, ROUGE );
	nP_send(&np, SpiHandle);
}
