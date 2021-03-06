/*
 * bh2221.c
 *
 *  Created on: Oct 28, 2020
 *      Author: sydxrey
 *
 * Controls the BH2221 12 channel 8bit DAC
 *

 */

/* Includes ------------------------------------------------------------------*/
#include "bh2221.h"
#include "main.h"
#include "misc.h"

/* External variables --------------------------------------------------------*/

extern SPI_HandleTypeDef *hspi_Dac;
extern DMA_HandleTypeDef *hdma_Dac_tx;


/* variables ---------------------------------------------------------*/

static uint8_t txDACBH2221Buff[3]; // 24 bit transmit buffer for DAC over SPI1
static uint16_t channel_data[16] = {0}; // one buffer for each DAC channel
static Boolean is_need_channel_data_sync[16] = {FALSE}; // list channels that need synchro to the DAC


/* function prototypes -----------------------------------------------*/


/* user code -----------------------------------------------*/


/**
 * Write the given word to the given channel buffer and mark data as needing sync.
 * Data need then to be sync with BH2221 device using BH2221_Write_Dma(), e.g., from a timer IRQ.
 */
void dacWrite1(uint16_t word12bits, Dac channel){ // TODO L432 vers F446 !!

	is_need_channel_data_sync[channel] = FALSE; // lock
	channel_data[channel] = word12bits;
	is_need_channel_data_sync[channel] = TRUE; // unlock

}

/**
 * Write the given word to the given DAC channel in blocking SPI mode.
 * Data are thus guaranteed to be written immediately to the BH2221 device.
 */
void dacWrite1_Blocking(uint16_t word, Dac channel){ // TODO L432 vers F446

	/* TODO L432 vers F446 :


	word &= 0xFFF; // make sure it's >=0 and <4096

	// send SYNC pulse (approx 560ns negative pulse, minimum duration in datasheet is 33ns so that's perfectly safe):
	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin;

	txDACBH2221Buff[0] = channel & 0x0F;
	txDACBH2221Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDACBH2221Buff[2] = ((word & 0x03F) << 2U);

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin;

	HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100); // TODO remove call to HAL, replace by registers

	*/
}


// =================================================================================
//                             BH2221
// =================================================================================



/**
 * Init the BH2221 device:
 * - select internal 2.5V voltage reference.
 * - initialize the SPI-DMA transfer
 * This is a blocking call.
 *
 *
 * BH2221 DAC initialization code :
 *
 * Write special function register : REG1=REG0=0
 * !A/B R/!W 0 0 A3 A2 A1 A0 . REG1 REG0 DB11 DB10 DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 DB1 DB0 X X
 *
 * 0 0 0 0 1 1 0 0 . 15:0 14:0 13:PDStatus 12:REFSelect 11:Boost 10:IntExtRef 9:Monitor 8:Thermal . 7-4 don't 3-2:Toggle 1-0 don't
 *
 * PDStatus : 1 => Amplifier output is high impedance in power down ; 0 => Amplifier output is 100k to ground in power down
 * REFSelect : 1 => internal ref is 2.5V ; 0 => internal ref is 1.25V
 */
void BH2221_Init_Device(){

	/* TODO L432 vers F446 :

	// send 560ns SYNC pulse, configuring tx buffer in the meantime:
	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin;
	txDACBH2221Buff[0] = 0x0C; // 0000.1100 (!A/B R/!W 0 0) . (A3 A2 A1 A0)
	txDACBH2221Buff[1] = 0x14; // 0001.0100 (REG1 REG0 PDStatus REFSelect) . (Boost IntExtRef Monitor Thermal)
	txDACBH2221Buff[2] = 0x00; // 0000.0000 (don't cares)
	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin;

	// send 3 bytes for special function register in blocking mode:
	HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100);

	// lower LDAC signal forever:
	DAC_LDAC_GPIO_Port->BRR = DAC_LDAC_Pin;

	// init SPI1/DMA transfer ; DMA transmit buffer is txDAC5391[3].
	SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_LDMATX); // transfer size is odd (1)

	__HAL_DMA_DISABLE(hdma_Dac_tx);

	hdma_Dac_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF3; // clear all pending interrupts
	hdma_Dac_tx->Instance->CNDTR = 3; // program 3 byte transfer
	hdma_Dac_tx->Instance->CPAR = (uint32_t)&(hspi_Dac->Instance->DR); // peripheral target address = SPI data register DR
	hdma_Dac_tx->Instance->CMAR = (uint32_t)txDACBH2221Buff; // memory source address

	__HAL_DMA_DISABLE_IT(hdma_Dac_tx, DMA_IT_HT); // half-transfer IT
	__HAL_DMA_DISABLE_IT(hdma_Dac_tx, DMA_IT_TC); // transfer complete IT
	__HAL_DMA_DISABLE_IT(hdma_Dac_tx, DMA_IT_TE); // transfer error IT

	// __HAL_DMA_ENABLE_IT(hdma_Dac_tx, (DMA_IT_TC | DMA_IT_TE)); // we don't actually make use of SPI interrupts actually (SR 4/30/2020)

	__HAL_DMA_ENABLE(hdma_Dac_tx); // re-enable DMA request
	__HAL_SPI_ENABLE(hspi_Dac); // enable SPI peripheral

	//__HAL_SPI_ENABLE_IT(spi_dac, (SPI_IT_TXE | SPI_IT_ERR)); // enable SPI TXE + ERR interrupt
	//__HAL_SPI_ENABLE_IT(spi_dac, SPI_IT_TXE);


	 */

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
void BH2221_Write_Dma(uint8_t channel){

	/* TODO L432 vers F446 :


	if (is_need_channel_data_sync[channel] == FALSE) return;

	uint16_t word = channel_data[channel]; // atomic copy?

	// send SYNC pulse (approx 560ns negative pulse, minimum duration in datasheet is 33ns so that's perfectly safe):
	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin;

	txDAC5391Buff[0] = channel & 0x0F;
	txDAC5391Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5391Buff[2] = ((word & 0x03F) << 2U);

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin;

	// trigger DMA transfer:
	__HAL_DMA_DISABLE(hdma_Dac_tx);
	hdma_Dac_tx->Instance->CNDTR = 3; // 3 bytes ; re-writing to CNDTR is enough to "restart" DMA request, see L4 or F4 datasheet
	__HAL_DMA_ENABLE(hdma_Dac_tx);
	SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_TXDMAEN); // re-enable Tx DMA request

	is_need_channel_data_sync[channel] = FALSE;

	*/
}




// =================================================================================
//                             TEST CODE
// =================================================================================














// ====================================================================================
// ARXIV
// ====================================================================================


// ============================ test code ====================================

// Polling transmit
/*void test_BH2221_Spi_Transmit(){


	while(1){
		DAC_SYNC_GPIO_Port->BRR = (uint32_t)(DAC_SYNC_Pin); // reset SYNC
		HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100);
		DAC_SYNC_GPIO_Port->BSRR = (uint32_t)DAC_SYNC_Pin; // set SYNC
		HAL_Delay(10);
	}


}*/

// DMA avec HAL
/*void test_BH2221_Spi_Transmit_DMA(){

	while(1){
		HAL_SPI_Transmit_DMA(hspi_Dac, txDAC5391Buff, 3);
		HAL_Delay(10);
	}
}*/

// DMA avec prog a la main des registres
/*void test_BH2221_Spi_Transmit_DMA_ManualConfig(){


	BH2221_Init_Dma();

	  while (1){
		  HAL_Delay(5);
		  BH2221_Transmit_Dma();
	  }
}*/

// test du DAC BH2221
/*void test_BH2221_Dac(){

	BH2221_Reset_Board();

	BH2221_Init_Dac();

	BH2221_Init_Dma();

	int t=0;
	uint32_t x;

	while(1){

		x = (uint32_t)(2000. * (1.0+sin(0.1 * t)));
		t++;
		BH2221_Write_Dma(x, 0x00);
		HAL_Delay(1);
	}

}*/

/**
 * Triggers one transfer of 3 bytes to the BH2221 DAC.
 * At 5Mbits/s, this takes 5us.
 */
/*static void BH2221_Transmit_SpiDma(){

	  __HAL_DMA_DISABLE(hdma_Dac_tx);
	  hdma_Dac_tx->Instance->CNDTR = 3;
	  __HAL_DMA_ENABLE(hdma_Dac_tx);
	  SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_TXDMAEN); // re-enable Tx DMA request

}*/


/*static void BH2221_Write_Blocking(uint32_t word, uint32_t channel){ // same as BH2221_Write_Dma  but blocking mode

	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin; // lower SYNC

	txDAC5391Buff[0] = channel & 0x0F;
	txDAC5391Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5391Buff[2] = ((word & 0x03F) << 2U);

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin; // raise SYNC (takes 560ns @ 80MHz)

	HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100);
}*/


