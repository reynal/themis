/*
 * ad5391.c
 *
 *  Created on: Apr 16, 2020
 *      Author: sydxrey
 *
 * Controls the AD5391 16 channel 12 bit DAC + MCP23017
 *
 *
 * SPI :
 * 	- pour avoir 24 bits d'affilee sans "trou", faire NSSPMode = SPI_NSS_PULSE_DISABLE dans CubeMX
 *		- attention, meme si Hardware NSS Signal = disable, ca compte quand meme !
 *
 *
 * AD5391 short doc: AD5391 is a 16 channel, 12 bit DAC with SPI interface.
 *
 * Transmission is over SPI1 with a 24bit word:
 *  !A/B R/!W 0 0 . A3 A2 A1 A0 . REG1 REG0 DB11 DB10 . DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 . DB1 DB0 X X
 *
 *  !A/B : For every channel there are two possible data registers, A and B, whose content can be written to the output DAC alternatively (see "Toggle mode" in datasheet, bits 3-2 in special function register)
 *  R/!W : 1 to readback register content ; 0 to write data to the DAC or the Special Function Register.
 *  A3 - A0 : channel address
 *  REG1 REG0:
 *   1    1     => input data register (=write data to DAC)
 *   1    0     => offset register
 *   0    1     => gain register
 *   0    0     => special function register (SFR), with A3-A0 telling what function to carry out
 *
 *   A3-A0 = 0xF : soft reset
 *   A3-A0 = 0xC : control register write
 *
 *   Control Register Write :
 *   0x0C . 15:0 14:0 13:PDStatus 12:REFSelect . 11:Boost 10:IntExtRef 9:Monitor 8:Thermal . 3-2:Toggle
 *
 *   To enable internal ref, must set IntExtRef=1
 *   To set Vref=2.5V, write REFSelect=1
 *
 *   Control Register thus should be:
 *   0x0C . 0 0 [0 1 . 0 1 0 0 . 0 0 0 0 . 0 0] 0 0 that is 0x0C 0x14 0x00
 *
 *
 *   Write 12bit word to output A3.A2.A1.A0 (channel A) :
 *   0x0 . A3 A2 A1 A0 . 1 1 DB11 DB10 . DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 . DB1 DB0 0 0
 *
 *
 *	 !Busy : goes low on 24th serial bit shifted in, goes up after only 600ns !
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ad5391.h"
#include "main.h"
#include "stdio.h"
#include "math.h"

/* External variables --------------------------------------------------------*/

extern SPI_HandleTypeDef *hspi_Dac;
extern DMA_HandleTypeDef *hdma_Dac_tx;
extern TIM_HandleTypeDef *htimDac;
extern I2C_HandleTypeDef *hi2c_MCP23017;
extern DMA_HandleTypeDef *hdma_MCP23017_tx;


/* variables ---------------------------------------------------------*/

uint8_t txDAC5391Buff[3]; // 24 bit transmit buffer for DAC over SPI1
uint32_t t;
uint32_t x;
uint8_t xIsValid;
uint32_t dac;

uint8_t mcp23017_gpioA_tx_Buff[2]; // {GPIOA register, value}
uint8_t mcp23017_gpioB_tx_Buff[2]; // {GPIOB register, value}


/* function prototypes -----------------------------------------------*/
static void ad5391_Reset_Board();
static void ad5391_Init_Dac();
static void ad5391_Write_Dma(uint32_t word, uint32_t channel);
static void ad5391_Init_Dma();
static void ad5391_Transmit_Dma();
void mcp23017_write_gpioA_dma();
void mcp23017_write_gpioB_dma();

/* user code -----------------------------------------------*/

// =================================================================================
//                             AD5391
// =================================================================================

/*
 * Calls what is necessary to init the DAC board
 */
void init_Dac_Board(){

	ad5391_Reset_Board(); // also MCP23017

	ad5391_Init_Dac();

	ad5391_Init_Dma();

}

/**
 * AD5391 DAC initialization code : select internal 2.5V voltage reference.
 *
 * Write special function register : REG1=REG0=0
 * !A/B R/!W 0 0 A3 A2 A1 A0 . REG1 REG0 DB11 DB10 DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 DB1 DB0 X X
 *
 * 0 0 0 0 1 1 0 0 . 15:0 14:0 13:PDStatus 12:REFSelect 11:Boost 10:IntExtRef 9:Monitor 8:Thermal . 7-4 don't 3-2:Toggle 1-0 don't
 *
 * PDStatus : 1 => Amplifier output is high impedance in power down ; 0 => Amplifier output is 100k to ground in power down
 * REFSelect : 1 => internal ref is 2.5V ; 0 => internal ref is 1.25V
 */
static void ad5391_Init_Dac(){


	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin; // lower SYNC

	txDAC5391Buff[0] = 0x0C;
	txDAC5391Buff[1] = 0x14;
	txDAC5391Buff[2] = 0x00;

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin; // raise SYNC (takes 560ns @ 80MHz)

	HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100); // blocking mode

	DAC_LDAC_GPIO_Port->BRR = DAC_LDAC_Pin; // lower LDAC

}

/**
 * init code for SPI1/DMA transfer. SPI1 is connected to DAC AD5391.
 * DMA transmit buffer is txDAC5391[3].
 */
static void ad5391_Init_Dma(){

	  // init DMA for SPI TX:
	  SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_LDMATX); // transfer size is even (0), or odd (1)

	  __HAL_DMA_DISABLE(hdma_Dac_tx);

	  hdma_Dac_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF3; // clear all pending interrupts
	  hdma_Dac_tx->Instance->CNDTR = 3; // program 3 byte transfer
	  hdma_Dac_tx->Instance->CPAR = (uint32_t)&(hspi_Dac->Instance->DR); // peripheral target address = SPI data register DR
	  hdma_Dac_tx->Instance->CMAR = (uint32_t)txDAC5391Buff; // memory source address

	  __HAL_DMA_DISABLE_IT(hdma_Dac_tx, DMA_IT_HT); // enable only transfer complete and transfer error
	  __HAL_DMA_ENABLE_IT(hdma_Dac_tx, (DMA_IT_TC | DMA_IT_TE));

	  __HAL_DMA_ENABLE(hdma_Dac_tx); // re-enable DMA request
	  __HAL_SPI_ENABLE(hspi_Dac); // enable SPI peripheral

	  //__HAL_SPI_ENABLE_IT(spi_dac, (SPI_IT_TXE | SPI_IT_ERR)); // enable SPI TXE + ERR interrupt
	  //__HAL_SPI_ENABLE_IT(spi_dac, SPI_IT_TXE);

	  //SET_BIT(spi_dac->Instance->CR2, SPI_CR2_TXDMAEN); // enable Tx DMA request
}

/**
 * Triggers one transfer of 3 bytes to the AD5391 DAC.
 * At 5Mbits/s, this takes 5us.
 */
static void ad5391_Transmit_Dma(){

	  __HAL_DMA_DISABLE(hdma_Dac_tx);
	  hdma_Dac_tx->Instance->CNDTR = 3;
	  __HAL_DMA_ENABLE(hdma_Dac_tx);
	  SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_TXDMAEN); // re-enable Tx DMA request

}


/**
 * Sends a negative RESET pulse to the AD5391 and MCP23017 circuits.
 */
static void ad5391_Reset_Board(){

	DAC_RST_GPIO_Port->BRR = (uint32_t)(DAC_RST_Pin); // lower RST
	HAL_Delay(1); // wait at leat 270us
	DAC_RST_GPIO_Port->BSRR = (uint32_t)DAC_RST_Pin; // raise RST again
	HAL_Delay(1); // necessary?

}



//
// Write 12bit word to output A3.A2.A1.A0 (channel A) :
// 0 0 0 0 A3 A2 A1 A0 . 1 1 DB11 DB10 DB9 DB8 DB7 DB6 . DB5 DB4 DB3 DB2 DB1 DB0 0 0
//
// DB11 DB10 DB9 DB8 . DB7 DB6 DB5 DB4 . DB3 DB2 DB1 DB0
//
// uint8_t txDAC5391_WRITECHANNEL[3] = {0x00, 0xC0, 0x00};
//
static void ad5391_Write_Dma(uint32_t word, uint32_t channel){

	// send SYNC pulse (approx 600ns negative pulse, minimum duration in datasheet is 33ns so that's perfectly safe):
	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin; // lower SYNC

	txDAC5391Buff[0] = channel & 0x0F;
	txDAC5391Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5391Buff[2] = ((word & 0x03F) << 2U);

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin; // raise SYNC (takes 560ns @ 80MHz)

	//HAL_SPI_Transmit(&hspi1, txDAC5391, 3, 100); (debug)

	// trigger DMA transfer:
	__HAL_DMA_DISABLE(hdma_Dac_tx);
	hdma_Dac_tx->Instance->CNDTR = 3; // 24 bits ; re-writing to CNDTR is enough to "restart" DMA request, see L4 or F4 datasheet
	__HAL_DMA_ENABLE(hdma_Dac_tx);
	SET_BIT(hspi_Dac->Instance->CR2, SPI_CR2_TXDMAEN); // re-enable Tx DMA request

}

/*static void ad5391_Write_Blocking(uint32_t word, uint32_t channel){ // same as ad5391_Write_Dma  but blocking mode

	DAC_SYNC_GPIO_Port->BRR = DAC_SYNC_Pin; // lower SYNC

	txDAC5391Buff[0] = channel & 0x0F;
	txDAC5391Buff[1] = 0xC0 | ((word & 0xFC0) >> 6U);
	txDAC5391Buff[2] = ((word & 0x03F) << 2U);

	DAC_SYNC_GPIO_Port->BSRR = DAC_SYNC_Pin; // raise SYNC (takes 560ns @ 80MHz)

	HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100);
}*/

/**
 * DAC IRQ handler that's fit for any timer.
 * Beware: at 5Mbits/s, a complete 24 bit transfer takes 5us and then
 * the busy signal goes low for 600ns hence minimum timer period must be above 6us.
 *
 */
void ad5391_TIM_IRQ(){

	//LD3_GPIO_Port->BRR = LD3_Pin; // debug
	if (dac==0) dac=1;
	else dac=0;

	ad5391_Write_Dma(x, dac);

	t++; // debug
	xIsValid = 0; // debug

	//LD3_GPIO_Port->BSRR = LD3_Pin; // debug
}

// ============================ test code ====================================

// Polling transmit
void test_AD5391_Spi_Transmit(){


	while(1){
		DAC_SYNC_GPIO_Port->BRR = (uint32_t)(DAC_SYNC_Pin); // reset SYNC
		HAL_SPI_Transmit(hspi_Dac, txDAC5391Buff, 3, 100);
		DAC_SYNC_GPIO_Port->BSRR = (uint32_t)DAC_SYNC_Pin; // set SYNC
		HAL_Delay(10);
	}


}

// DMA avec HAL
void test_AD5391_Spi_Transmit_DMA(){

	while(1){
		HAL_SPI_Transmit_DMA(hspi_Dac, txDAC5391Buff, 3);
		HAL_Delay(10);
	}
}

// DMA avec prog a la main des registres
void test_AD5391_Spi_Transmit_DMA_ManualConfig(){


	ad5391_Init_Dma();

	  while (1){
		  HAL_Delay(5);
		  ad5391_Transmit_Dma();
	  }
}

// test du DAC AD5391
void test_AD5391_Dac(){

	ad5391_Reset_Board();

	ad5391_Init_Dac();

	ad5391_Init_Dma();

	int t=0;
	uint32_t x;

	while(1){

		x = (uint32_t)(2000. * (1.0+sin(0.1 * t)));
		t++;
		ad5391_Write_Dma(x, 0x00);
		HAL_Delay(1);
	}

}

void test_AD5391_Dac_With_Tim(){

	t=0;
	dac=0;
	xIsValid = 0;

	ad5391_Reset_Board();

	ad5391_Init_Dac();

	ad5391_Init_Dma();

	// init TIMER DAC:
	__HAL_TIM_ENABLE_IT(htimDac, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(htimDac);

	while (1){

		if (xIsValid == 0){
			x = (uint32_t)(2000. * (1.0+sin(0.1 * t)));
			xIsValid = 1;
		}
		//HAL_Delay(1);
	}
}

// =================================================================================
//                             MCP23017
// =================================================================================

/*
 * MCP23017 device
 *
 * GPA0 : 411_IN_PULSE
 * GPA1 : 411_IN_TRI
 * GPA2 : 411_IN_SYNC
 * GPA3 : 411_IN_SAW
 * GPA4 : VCF_2ND_ORDER
 * GPA5 : VCF_4TH_ORDER
 * GPA6 : Connecteur CONN_MCP_A : 1 (en haut tout à droite, 1 puis 2)
 * GPA7 : idem : 2
 *
 * GPB0 : BD
 * GPB1 : Snare
 * GPB2 : RIM
 * GPB3 : TOM1
 * GPB4 : TOM2
 * GPB5 : Connecteur CONN_MCP_B : 1 (juste à gauche de l'alimentation, 3 puis 2 puis 1 de G à D)
 * GPB6 : idem : 2
 * GPB7 : idem : 3
 *
 * Important registers:
 *
 * Note : IOCON.BANK = 0 by default
 * - IODIRA : addr=0x00 ; 0=>OUTPUT, 1=>INPUT (def)
 * - IODIRB : addr=0x01
 * - GPIOA  : addr=0x12
 * - GPIOB  : addr=0x13
 *
 *
 * I2C Control byte format: S | 0 1 0 0 A2 A1 A0 R/!W | ACK
 * 		where S = start condition and the next 8 bits are called "OP" (OP device)
 * 		and ACK = acknowledgement provided by the MCP23017
 *
 * 		We have A2=A1=A0=0 here (see eagle schematics)
 *
 * 		For instance, write to register GPIOA :
 * 		START | 0x40 0x00 | ACK | Register addr 0x12 | ACK | Register value | STOP
 *
 */

void test_MCP23017(){

	uint8_t configBuff[2];

	ad5391_Reset_Board();

	// Set IODIRA and IODIRB as output ports:

	configBuff[0] = MCP23017_REG_IODIRA; // Register address
	configBuff[1] = MCP23017_IODIR_ALL_OUTPUT; // Value
	HAL_I2C_Master_Transmit(hi2c_MCP23017, MCP23017_ADDRESS, configBuff, 2, 100); // blocking call

	configBuff[0] = MCP23017_REG_IODIRB;
	HAL_I2C_Master_Transmit(hi2c_MCP23017, MCP23017_ADDRESS, configBuff, 2, 100); // blocking call

	mcp23017_gpioA_tx_Buff[0] = MCP23017_REG_GPIOA;
	mcp23017_gpioB_tx_Buff[0] = MCP23017_REG_GPIOB;

	// init DMA transfer for DMA1 Channel2 (I2C3):

	__HAL_DMA_DISABLE(hdma_MCP23017_tx);
    hdma_MCP23017_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF2; // clear interrupt flag
    hdma_MCP23017_tx->Instance->CNDTR = 2; /* Configure DMA Channel data length */
    hdma_MCP23017_tx->Instance->CPAR = (uint32_t)&(hi2c_MCP23017->Instance->TXDR);
    __HAL_DMA_DISABLE_IT(hdma_MCP23017_tx, DMA_IT_HT | DMA_IT_TC | DMA_IT_TE); // no IT

  	//LD3_GPIO_Port->BRR = LD3_Pin; // debug
  	//LD3_GPIO_Port->BSRR = LD3_Pin; // debug


	while (1){

		mcp23017_gpioA_tx_Buff[1]=0xFF;
		mcp23017_write_gpioA_dma();
		HAL_Delay(1);

		mcp23017_gpioA_tx_Buff[1]=0x00;
		mcp23017_write_gpioA_dma();
		HAL_Delay(1);

		mcp23017_gpioB_tx_Buff[1]=0xFF;
		mcp23017_write_gpioB_dma();
		HAL_Delay(1);

		mcp23017_gpioB_tx_Buff[1]=0x00;
		mcp23017_write_gpioB_dma();
		HAL_Delay(1);
	}
}

void mcp23017_write_gpioA_dma(){

	__HAL_DMA_DISABLE(hdma_MCP23017_tx);
    hdma_MCP23017_tx->Instance->CNDTR = 2;
    hdma_MCP23017_tx->Instance->CMAR = (uint32_t)mcp23017_gpioA_tx_Buff;
    __HAL_DMA_ENABLE(hdma_MCP23017_tx);

    // send slave address:
  	MODIFY_REG(hi2c_MCP23017->Instance->CR2,
  			  ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | (I2C_CR2_RD_WRN & (uint32_t)(I2C_GENERATE_START_WRITE >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
			  (uint32_t)(((uint32_t)MCP23017_ADDRESS & I2C_CR2_SADD) | (((uint32_t)2 << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)I2C_AUTOEND_MODE | (uint32_t)I2C_GENERATE_START_WRITE));

  	// send data:
  	hi2c_MCP23017->Instance->CR1 |= I2C_CR1_TXDMAEN;
}

void mcp23017_write_gpioB_dma(){

	__HAL_DMA_DISABLE(hdma_MCP23017_tx);
    hdma_MCP23017_tx->Instance->CNDTR = 2;
    hdma_MCP23017_tx->Instance->CMAR = (uint32_t)mcp23017_gpioB_tx_Buff;
    __HAL_DMA_ENABLE(hdma_MCP23017_tx);

    // send slave address:
  	MODIFY_REG(hi2c_MCP23017->Instance->CR2,
  			  ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | (I2C_CR2_RD_WRN & (uint32_t)(I2C_GENERATE_START_WRITE >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
			  (uint32_t)(((uint32_t)MCP23017_ADDRESS & I2C_CR2_SADD) | (((uint32_t)2 << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)I2C_AUTOEND_MODE | (uint32_t)I2C_GENERATE_START_WRITE));

  	// send data:
  	hi2c_MCP23017->Instance->CR1 |= I2C_CR1_TXDMAEN;


}

// from https://github.com/ruda/mcp23017/blob/master/src/mcp23017.c

HAL_StatusTypeDef mcp23017_write(uint16_t reg, uint8_t *data){

	return HAL_I2C_Mem_Write(hi2c_MCP23017, MCP23017_ADDRESS, reg, 1, data, 1, 10);

}




