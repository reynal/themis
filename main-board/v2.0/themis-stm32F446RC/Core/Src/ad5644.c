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
 *
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

/* External variables --------------------------------------------------------*/

extern SPI_HandleTypeDef *hspi_Dac;
extern DMA_HandleTypeDef *hdma_Dac_tx;


/* variables ---------------------------------------------------------*/

static uint8_t txDAC5644Buff[3]; // 24 bit transmit buffer for DAC over SPI
static uint16_t channel_data[AD5644_CHANNEL_COUNT] = {0}; // one buffer for each DAC channel
static Boolean is_need_channel_data_sync[AD5644_CHANNEL_COUNT] = {FALSE}; // list channels that need synchro to the DAC


/* function prototypes -----------------------------------------------*/


/* user code -----------------------------------------------*/


/**
 * Write the given word to the given channel buffer and mark data as needing sync.
 * Data need then to be sync with AD5644 device using ad5644_Write_Dma(), e.g., from a timer IRQ.
 */
void dacWrite(uint16_t word14bits, Dac channel){

	is_need_channel_data_sync[channel] = FALSE; // lock
	channel_data[channel] = word14bits;
	is_need_channel_data_sync[channel] = TRUE; // unlock

}

/**
 * Write the given word to the given DAC channel in blocking SPI mode.
 * Data are thus guaranteed to be written immediately to the AD5644 device.
 */
void dacWrite_Blocking(uint16_t word, Dac channel){

	word &= 0x3FFF; // make sure it's >=0 and <16384

	// send SYNC pulse (approx 560ns negative pulse, minimum duration in datasheet is 33ns so that's perfectly safe):
	//AD5644_SYNC_GPIO_Port->BSRR = (uint32_t)AD5644_SYNC_Pin << 16U; // =
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);

	txDAC5644Buff[0] = channel & 0x0F;
	txDAC5644Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5644Buff[2] = ((word & 0x03F) << 2U);


	HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100); // TODO remove call to HAL, replace by registers

	//AD5644_SYNC_GPIO_Port->BSRR = AD5644_SYNC_Pin; // =
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
}


// =================================================================================
//                             AD5644
// =================================================================================

static void ad5644_sw_reset(){

	/* was AD5391
	// send 560ns SYNC pulse, configuring tx buffer in the meantime:
	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin;
	txDAC5644Buff[0] = 0x0C; // 0000.1100 (!A/B R/!W 0 0) . (A3 A2 A1 A0)
	txDAC5644Buff[1] = 0x14; // 0001.0100 (REG1 REG0 PDStatus REFSelect) . (Boost IntExtRef Monitor Thermal)
	txDAC5644Buff[2] = 0x00; // 0000.0000 (don't cares)
	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin;
	// send 3 bytes for special function register in blocking mode:
	HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100);*/

	txDAC5644Buff[0]=0x28;
	txDAC5644Buff[1]=0x00;
	txDAC5644Buff[2]=0x01;
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

}

static void ad5644_internal_vref_on(){

	txDAC5644Buff[0]=0x38;
	txDAC5644Buff[1]=0x00;
	txDAC5644Buff[2]=0x01;
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

static void ad5644_ldac_autoupdate(){

	txDAC5644Buff[0]=0x30;
	txDAC5644Buff[1]=0x00;
	txDAC5644Buff[2]=0x0F;
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(AD5644_SYNC_GPIO_Port, AD5644_SYNC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

}

static void ad5644_init_dma(){
	/* TODO L432 vers F446 :
	// init SPI1/DMA transfer ; DMA transmit buffer is txDAC5644[3].
	SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_LDMATX); // transfer size is odd (1)

	__HAL_DMA_DISABLE(hdma_Dac_tx);

	hdma_Dac_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF3; // clear all pending interrupts
	hdma_Dac_tx->Instance->CNDTR = 3; // program 3 byte transfer
	hdma_Dac_tx->Instance->CPAR = (uint32_t)&(hspi_Dac->Instance->DR); // peripheral target address = SPI data register DR
	hdma_Dac_tx->Instance->CMAR = (uint32_t)txDAC5644Buff; // memory source address

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
 * Init the AD5644 device:
 * - select internal 2.5V voltage reference.
 * - initialize the SPI-DMA transfer
 * This is a blocking call.
 *
 *
 * AD5644 DAC initialization code :
 *
 *
 *
 */
void ad5644_Init_Device(){

	ad5644_sw_reset();
	ad5644_internal_vref_on();
	ad5644_ldac_autoupdate();
	ad5644_init_dma();

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
void ad5644_Write_Dma(uint8_t channel){
/* TODO L432 vers F446 :
	if (is_need_channel_data_sync[channel] == FALSE) return;

	uint16_t word = channel_data[channel]; // atomic copy?

	// send SYNC pulse (approx 560ns negative pulse, minimum duration in datasheet is 33ns so that's perfectly safe):
	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin;

	txDAC5644Buff[0] = channel & 0x0F;
	txDAC5644Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5644Buff[2] = ((word & 0x03F) << 2U);

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
/*void test_AD5644_Spi_Transmit(){


	while(1){
		DAC_SYNC_GPIO_Port->BRR = (uint32_t)(DAC_SYNC_Pin); // reset SYNC
		HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100);
		DAC_SYNC_GPIO_Port->BSRR = (uint32_t)DAC_SYNC_Pin; // set SYNC
		HAL_Delay(10);
	}


}*/

// DMA avec HAL
/*void test_AD5644_Spi_Transmit_DMA(){

	while(1){
		HAL_SPI_Transmit_DMA(hspi_Dac, txDAC5644Buff, 3);
		HAL_Delay(10);
	}
}*/

// DMA avec prog a la main des registres
/*void test_AD5644_Spi_Transmit_DMA_ManualConfig(){


	ad5644_Init_Dma();

	  while (1){
		  HAL_Delay(5);
		  ad5644_Transmit_Dma();
	  }
}*/

// test du DAC AD5644
/*void test_AD5644_Dac(){

	ad5644_Reset_Board();

	ad5644_Init_Dac();

	ad5644_Init_Dma();

	int t=0;
	uint32_t x;

	while(1){

		x = (uint32_t)(2000. * (1.0+sin(0.1 * t)));
		t++;
		ad5644_Write_Dma(x, 0x00);
		HAL_Delay(1);
	}

}*/

/**
 * Triggers one transfer of 3 bytes to the AD5644 DAC.
 * At 5Mbits/s, this takes 5us.
 */
/*static void ad5644_Transmit_SpiDma(){

	  __HAL_DMA_DISABLE(hdma_Dac_tx);
	  hdma_Dac_tx->Instance->CNDTR = 3;
	  __HAL_DMA_ENABLE(hdma_Dac_tx);
	  SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_TXDMAEN); // re-enable Tx DMA request

}*/


/*static void ad5644_Write_Blocking(uint32_t word, uint32_t channel){ // same as ad5644_Write_Dma  but blocking mode

	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin; // lower SYNC

	txDAC5644Buff[0] = channel & 0x0F;
	txDAC5644Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5644Buff[2] = ((word & 0x03F) << 2U);

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin; // raise SYNC (takes 560ns @ 80MHz)

	HAL_SPI_Transmit(hspi_Dac, txDAC5644Buff, 3, 100);
}*/


