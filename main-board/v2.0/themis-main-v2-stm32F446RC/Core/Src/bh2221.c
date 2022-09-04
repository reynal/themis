/*
 * bh2221.c
 *
 *  Created on: Oct 28, 2020
 *      Author: sydxrey
 *
 * Controls the BH2221 12 channel 8bit DAC
 *
 * This device is very simple to program and doesn't need any specific initialization, as opposed to the AD5644 device.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "bh2221.h"
#include "main.h"
#include "misc.h"
#include <stdbool.h>

/* External variables --------------------------------------------------------*/

extern SPI_HandleTypeDef hspi1;

/* variables ---------------------------------------------------------*/

static uint8_t txDacBuf[2]= {0x0D, 0x80}; // 16 bit transmit buffer for DAC over SPI1
static uint16_t channel_data[BH2221_CHANNEL_COUNT] = {0}; // one buffer for each DAC channel
static bool is_need_channel_data_sync[BH2221_CHANNEL_COUNT] = {false}; // list channels that need synchro to the DAC

static uint8_t channel_codes[] = {0x08, 0x04, 0x0C, 0x02, 0x0A, 0x06, 0x0E, 0x01, 0x09, 0x05, 0x0D, 0x03}; // values for txDACBH2221Buff[0], aka channel index (from AO1 to AO12)

/* function prototypes -----------------------------------------------*/


/* user code -----------------------------------------------*/

/**
 * Init DMA registers for future transactions.
 */
void bh2221InitDMA(){

	HAL_GPIO_WritePin(BH2221_LD_GPIO_Port, BH2221_LD_Pin, GPIO_PIN_SET); // make sure LD is high before writing to DAC

	txDacBuf[0] = 0x0D; // channel AO11
	txDacBuf[1] = 128; // midscale arbitrary value to begin with

	// __NOP(); // if pulse's too short on LD_GPIO ; insert as many as needed

	HAL_GPIO_WritePin(BH2221_LD_GPIO_Port, BH2221_LD_Pin, GPIO_PIN_RESET); // taking LD low enable data-in shifting

	hspi_BH2221->hdmatx->Instance->NDTR = 2;
	hspi_BH2221->hdmatx->Instance->PAR = (uint32_t)&(hspi_BH2221->Instance->DR);
	hspi_BH2221->hdmatx->Instance->M0AR = (uint32_t)txDacBuf;

	// clear all IF by writing to LIFCR register (Int Flag CR):
	DMA_REG *regs = (DMA_REG *)hspi_BH2221->hdmatx->StreamBaseAddress;
	regs->IFCR = 0x3FU << hspi_BH2221->hdmatx->StreamIndex; // clear all IF
	// DMA2->LIFCR = 0x3FU << 22; // ibid as above but through direct register addressing (DMA2 for Stream 3 and 7, otherwise it's DMA1)
	__HAL_DMA_ENABLE(hspi_BH2221->hdmatx);
	__HAL_SPI_ENABLE(hspi_BH2221); // 1st time only

	SET_BIT(hspi_BH2221->Instance->CR2, SPI_CR2_TXDMAEN);

	HAL_Delay(1);

}

/**
 * Write the given byte to the given channel buffer and mark data as needing sync.
 * Data need then to be sync with BH2221 device using BH2221_Xfer_Buffer_Dma(), e.g., from a timer IRQ.
 */
void bh2221WriteAsync(uint8_t byte, bh2221Channel_e channel){

	is_need_channel_data_sync[channel] = false; // lock
	channel_data[channel] = byte;
	is_need_channel_data_sync[channel] = true; // unlock

}

/**
 * Write the given byte to the given DAC channel in blocking SPI mode.
 * Data are thus guaranteed to be written immediately to the BH2221 device.
 */
void bh2221WriteBlocking(uint8_t val8, bh2221Channel_e channel){

	// BH2221FV DAC : SPI1 (settling time = 100us for a 0 -> 255 output jump, otherwise it's shorter)
	// CLK = PA5
	// MOSI = PA7
	// (NSS = PA4 not used yet => BH2221 LD signal)

	txDacBuf[0]=channel_codes[channel];
	txDacBuf[1]=val8;
	HAL_GPIO_WritePin(BH2221_LD_GPIO_Port, BH2221_LD_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi_BH2221, txDacBuf, 2, 100) != HAL_OK) Error_Handler();

	HAL_GPIO_WritePin(BH2221_LD_GPIO_Port, BH2221_LD_Pin, GPIO_PIN_SET);

	__NOP();
	__NOP();
	__NOP();

}

/**
 * Write a byte to output A3.A2.A1.A0 (channel A) :
 * 0 0 0 0 A3 A2 A1 A0 . 1 1 DB11 DB10 DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 DB1 DB0 0 0
 *
 * Beware: non blocking call! Should be called from a timer IRQ handler.
 *
 * Timing: at 5Mbits/s SPI baud rate, a complete 16 bit transfer takes ~3us
 *
 */
void bh2221FlushBufferDma(bh2221Channel_e channel){

	if (is_need_channel_data_sync[channel] == false) return;

	HAL_GPIO_WritePin(BH2221_LD_GPIO_Port, BH2221_LD_Pin, GPIO_PIN_SET); // Latch previously written byte

	txDacBuf[0] = channel_codes[channel];
	txDacBuf[1] = channel_data[channel]; // 8bits

	// __NOP(); // if pulse's too short on LD_GPIO ; insert as many as needed

	HAL_GPIO_WritePin(BH2221_LD_GPIO_Port, BH2221_LD_Pin, GPIO_PIN_RESET); // Go down to enable data in shifting

	// The following 4 lines are enough to retrigger a DMA transfer provided we won't use IRQs:
	__HAL_DMA_DISABLE(hspi_BH2221->hdmatx);
	// clear all IF for DMA2/Stream3 by writing 1111X1 at proper position in DMA_LIFCR, otherwise transfer won't restart :-/
	// (note that this is normally done inside HAL's IRQ Handler, but since we didn't enable ITs, it won't be called...)
	DMA_REG *regs = (DMA_REG *)hspi_BH2221->hdmatx->StreamBaseAddress;
	regs->IFCR = 0x3FU << hspi_BH2221->hdmatx->StreamIndex; // clear all IF
	//DMA2->LIFCR = 0x3FU << 22;
	hspi_BH2221->hdmatx->Instance->NDTR = 2;

	__HAL_DMA_ENABLE(hspi_BH2221->hdmatx);

	is_need_channel_data_sync[channel] = false;

}


void bh2221Test(){

	uint8_t val8=100;
	uint8_t channel = BH2221_AO12;

	bh2221InitDMA();

	while(1){
		//bh2221_Write_Blocking(val8,0);

		bh2221WriteAsync(val8, channel);
		bh2221FlushBufferDma(channel);

		val8+=2;
		if (val8 >= 0xFF) val8=0;
		HAL_Delay(1);
	}

}

