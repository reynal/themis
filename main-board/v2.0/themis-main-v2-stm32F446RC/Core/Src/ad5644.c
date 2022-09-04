/*
 * ad5644.c
 *
 *  Created on: Apr 16, 2020 (ad5391.c), adapted to ad5644.c Oct 28, 2020
 *      Author: sydxrey
 *
 * Controls the AD5644 4channel 14bit DAC (very similar to AD5391)
 *
 *
 * SPI :
 * 	- pour avoir 24 bits d'affilee sans "trou", faire NSSPMode = SPI_NSS_PULSE_DISABLE dans CubeMX
 *		- attention, meme si Hardware NSS Signal = disable, ca compte quand meme !
 *		ADDENDUM : only for L432? also for F446?
 *
 *
 *
 * =================
 * AD5644 short doc:
 *
 * input shift register (24 bits wide):
 * 0 0 C2 C1 . C0 A2 A1 A0 . D13-D10 . D9-6 . D5-2 . D1 D0 0 0
 *
 * SW RESET :
 * 0010 1000 0000 0000 0000 0001 = 0x280001
 *
 * Internal VRef ON:
 * 0011 1000 0000 0000 0000 0001 = 0x380001
 *
 * Setting up LDAC register : (DAC regs updated on the falling edge of the 24th clk pulse)
 * 0011 0000 0000 0000 0000 1111 = 0x30000F
 *
 * Write "val" (14bits) to DAC nn (nn=0,1,2,3) :
 * 0000 00nn D13-D0 00 = 0x07 WX YZ, where WX = (val >> 6) & 0x00FF and YZ = (val << 2) & 0x00FF
 *
 *  */

/* Includes ------------------------------------------------------------------*/
#include "ad5644.h"
#include "main.h"
#include "misc.h"
#include <stdbool.h>

/* External variables --------------------------------------------------------*/

extern SPI_HandleTypeDef hspi2;


/* variables ---------------------------------------------------------*/

static uint8_t txDacBuf[3]; // 24 bit transmit buffer for DAC over SPI
static uint16_t channel_data[AD5644_CHANNEL_COUNT] = {0}; // one buffer for each DAC channel
static bool is_need_channel_data_sync[AD5644_CHANNEL_COUNT] = {false}; // list channels that need synchro to the DAC

static bool is_DMA_initialized;


/* function prototypes -----------------------------------------------*/

static void ad5644SwReset();
static void ad5644InternalVrefOn();
static void ad5644LdacAutoupdate();
static void ad5644InitDma();

/* user code -----------------------------------------------*/

/**
 * Init the AD5644 device:
 * - select internal 2.5V voltage reference.
 * - initialize the SPI-DMA transfer
 * This is a blocking call.
 */
void ad5644Init(){

	is_DMA_initialized=false;
	ad5644SwReset();
	ad5644InternalVrefOn();
	ad5644LdacAutoupdate();
	ad5644InitDma();

}


/**
 * Write the given word to the given channel buffer and mark data as needing sync.
 * Data need then to be sync with AD5644 device using ad5644_Write_Dma(), e.g., from a timer IRQ.
 */
void ad5644WriteAsync(uint16_t word14bits, ad5644Channel_e channel){

	is_need_channel_data_sync[channel] = false; // lock
	channel_data[channel] = word14bits;
	is_need_channel_data_sync[channel] = true; // unlock

}

/**
 * Write the given word to the given DAC channel in blocking SPI mode.
 * Data are thus guaranteed to be written immediately to the AD5644 device.
 * @param channel 0 to 3
 */
void ad5644WriteBlocking(uint16_t val14, ad5644Channel_e channel){

	val14 &= 0x3FFF; // make sure it's >=0 and <16384

	//txDAC5644Buff[0]=0x07; // (cf page 21 de la datasheet) : 0000 0111 => C2C1C0=000 (write to input reg) et A2A1A0=111 (update all DACs registers)
	txDacBuf[0] = channel & 0x07;
	txDacBuf[1]=(val14 >> 6) & 0x00FF;
	txDacBuf[2]=(val14 << 2) & 0x00FF;
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET); // doc p.21: SYNC must be brough high for a min of 15ns before the next write seq (that starts with a LOW)

	if (HAL_SPI_Transmit(hspi_AD5644, txDacBuf, 3, 100) != HAL_OK) Error_Handler();
	//if (HAL_SPI_Transmit_DMA(hspi_AD5644, txDAC5644Buff, 3) != HAL_OK) Error_Handler(); // validé le 24 juin 2022
	//HAL_Delay(1);

	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);

}


// =================================================================================
//                             AD5644
// =================================================================================

static void ad5644SwReset(){

	txDacBuf[0]=0x28; // C2 C1 C0 = 101 = RESET (page 21)
	txDacBuf[1]=0x00; // N/A
	txDacBuf[2]=0x01; // Perform a true Power-On reset, not just a SW reset (that is, reset every register)
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(hspi_AD5644, txDacBuf, 3, 100) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

}

static void ad5644InternalVrefOn(){

	txDacBuf[0]=0x38; // C2 C1 C0 = 111 = Internal Reference Setup
	txDacBuf[1]=0x00; // N/A
	txDacBuf[2]=0x01; // Turn ON internal ref
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(hspi_AD5644, txDacBuf, 3, 100) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

static void ad5644LdacAutoupdate(){

	txDacBuf[0]=0x30; // C2 C1 C0 = 110 = LDAC register setup
	txDacBuf[1]=0x00; // N/A
	txDacBuf[2]=0x0F; // all 4 DAC registers are transparent (i.e. DAC is updated immediately: the contents of the input registers are transferred to the DAC on the falling edge of the 24th SCLK pulse)
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(hspi_AD5644, txDacBuf, 3, 100) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

}



static void ad5644InitDma(){

	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);

	uint16_t val14  = 8192;

	//txDAC5644Buff[0]=0x07; // (cf page 21 de la datasheet) : 0000 0111 => C210=000 (write to reg) et A210=111 update all DACs registers
	txDacBuf[0] = 0; // arbitrary
	txDacBuf[1]=(val14 >> 6) & 0x00FF;
	txDacBuf[2]=(val14 << 2) & 0x00FF;

	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);

	hspi_AD5644->hdmatx->Instance->NDTR = 3;
	hspi_AD5644->hdmatx->Instance->PAR = (uint32_t)&(hspi_AD5644->Instance->DR);
	hspi_AD5644->hdmatx->Instance->M0AR = (uint32_t)txDacBuf;

	// clear all IF by writing to LIFCR register (Int Flag CR):
	DMA_REG *regs = (DMA_REG *)hspi_AD5644->hdmatx->StreamBaseAddress;
	regs->IFCR = 0x3FU << hspi_AD5644->hdmatx->StreamIndex; // clear all IF
	// DMA2->LIFCR = 0x3FU << 22; // ibid as above but through direct register addressing (DMA2 for Stream 3 and 7, otherwise it's DMA1)
	__HAL_DMA_ENABLE(hspi_AD5644->hdmatx);
	__HAL_SPI_ENABLE(hspi_AD5644); // 1st time only

	SET_BIT(hspi_AD5644->Instance->CR2, SPI_CR2_TXDMAEN);
}

/**
 * Write 12bit word to output A3.A2.A1.A0 (channel A) :
 * 0 0 0 0 A3 A2 A1 A0 . 1 1 DB11 DB10 DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 DB1 DB0 0 0
 *
 * Beware: non blocking call! Should be called from a timer IRQ handler.
 *
 * Timing:
 * - SYNC pin low pulse takes 560ns @ 80MHz clock
 * - at 5Mbits/s SPI baud rate, a complete 24 bit transfer takes 5us and then
 *   the busy signal goes low for 600ns hence minimum timer period must be above 6us.
 *
 */
void ad5644FlushBufferDma(ad5644Channel_e channel){

	if (is_need_channel_data_sync[channel] == false) return;

	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);

	uint16_t val14  = channel_data[channel] & 0x3FFF; // atomic copy? Also make sure it's >=0 and <16384

	//txDAC5644Buff[0]=0x07; // (cf page 21 de la datasheet) : 0000 0111 => C210=000 (write to reg) et A210=111 update all DACs registers
	txDacBuf[0] = channel & 0x07;
	txDacBuf[1]=(val14 >> 6) & 0x00FF;
	txDacBuf[2]=(val14 << 2) & 0x00FF;

	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);

	//same as HAL_SPI_Transmit_DMA(hspi_AD5644, _spiBuf, TLC_BUF_SZ) but optimized:

	// The following 4 lines are enough to retrigger a DMA transfer provided we won't use IRQs:
	__HAL_DMA_DISABLE(hspi_AD5644->hdmatx);
	// clear all IF for DMA2/Stream3 by writing 1111X1 at proper position in DMA_LIFCR, otherwise transfer won't restart :-/
	// (note that this is normally done inside HAL's IRQ Handler, but since we didn't enable ITs, it won't be called...)
	DMA_REG *regs = (DMA_REG *)hspi_AD5644->hdmatx->StreamBaseAddress;
	regs->IFCR = 0x3FU << hspi_AD5644->hdmatx->StreamIndex; // clear all IF
	//DMA2->LIFCR = 0x3FU << 22;
	hspi_AD5644->hdmatx->Instance->NDTR = 3;

	__HAL_DMA_ENABLE(hspi_AD5644->hdmatx);

	is_need_channel_data_sync[channel] = false;
}

// hardware test:
void ad5644Test(){

	ad5644Init();

	uint16_t val14=1000;
	int channel = 3;

	while(1){
		//ad5644_Write_Blocking(val14,0);

		ad5644WriteAsync(val14, channel);
		ad5644FlushBufferDma(channel);

		val14+=128;
		if (val14 >= 0x3FFF) val14=0;
		HAL_Delay(1);
	}

}
