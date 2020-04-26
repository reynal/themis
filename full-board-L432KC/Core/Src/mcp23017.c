/*
 * mcp23017.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 *
 *
 *
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
 *
 * 	Timings:
 * 	- 2 bytes transmission over I2C at 1MHz (fast mode plus) takes ~31us.
 * 	- Call to mcp23017_Write_GpioA/B_Dma() takes 1.5us @ 80MHz CPU clock
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "mcp23017.h"
#include "stm32l4xx_hal.h"
#include "misc.h"

/* External variables --------------------------------------------------------*/

extern I2C_HandleTypeDef *hi2c_MCP23017;
extern DMA_HandleTypeDef *hdma_MCP23017_tx;


/* variables ---------------------------------------------------------*/

uint8_t mcp23017_gpioA_tx_Buff[2]; // {GPIOA register, value}
uint8_t mcp23017_gpioB_tx_Buff[2]; // {GPIOB register, value}

Boolean is_gpioA_buffer_need_sync; // set to false as soon as mcp23017_gpioA_tx_Buff[1] is being written by another thread
Boolean is_gpioB_buffer_need_sync;

/* function prototypes -----------------------------------------------*/



/* user code -----------------------------------------------*/

/*
 * Write the content of mcp23017_gpioA_tx_Buff[1] to GPIO port A
 * Beware! Non-blocking call (takes 1.5us @ 80MHz CPU clock). Should be called from a TIMER IRQ.
 */
void mcp23017_Tx_GpioA_Buffer_Dma(){

	if (is_gpioA_buffer_need_sync == FALSE) return;

	__HAL_DMA_DISABLE(hdma_MCP23017_tx);
    hdma_MCP23017_tx->Instance->CNDTR = 2; // TODO : necessary?
    hdma_MCP23017_tx->Instance->CMAR = (uint32_t)mcp23017_gpioA_tx_Buff;
    __HAL_DMA_ENABLE(hdma_MCP23017_tx);

    // send slave address:
  	MODIFY_REG(hi2c_MCP23017->Instance->CR2,
  			  ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | (I2C_CR2_RD_WRN & (uint32_t)(I2C_GENERATE_START_WRITE >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
			  (uint32_t)(((uint32_t)MCP23017_ADDRESS & I2C_CR2_SADD) | (((uint32_t)2 << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)I2C_AUTOEND_MODE | (uint32_t)I2C_GENERATE_START_WRITE));

  	// trigger DMA transfer of data:
  	hi2c_MCP23017->Instance->CR1 |= I2C_CR1_TXDMAEN;

  	is_gpioA_buffer_need_sync = FALSE;
}

/*
 * Write the content of mcp23017_gpioB_tx_Buff[1] to GPIO port B
 * Beware! Non-blocking call (takes 1.5us @ 80MHz CPU clock). Should be called from a TIMER IRQ.
 */
void mcp23017_Tx_GpioB_Buffer_Dma(){

	if (is_gpioB_buffer_need_sync == FALSE) return;

	__HAL_DMA_DISABLE(hdma_MCP23017_tx);
    hdma_MCP23017_tx->Instance->CNDTR = 2;
    hdma_MCP23017_tx->Instance->CMAR = (uint32_t)mcp23017_gpioB_tx_Buff;
    __HAL_DMA_ENABLE(hdma_MCP23017_tx);

    // send slave address:
  	MODIFY_REG(hi2c_MCP23017->Instance->CR2,
  			  ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | (I2C_CR2_RD_WRN & (uint32_t)(I2C_GENERATE_START_WRITE >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
			  (uint32_t)(((uint32_t)MCP23017_ADDRESS & I2C_CR2_SADD) | (((uint32_t)2 << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)I2C_AUTOEND_MODE | (uint32_t)I2C_GENERATE_START_WRITE));

  	// trigger DMA transfer of data:
  	hi2c_MCP23017->Instance->CR1 |= I2C_CR1_TXDMAEN;

  	is_gpioB_buffer_need_sync = FALSE;


}

/*
 * Init the MCP23017 device:
 * - set all pins in ports A and B as outputs
 * - initialize the DMA transfer
 * This is a blocking call.
 */
void mcp23017_Init_Device(){

	is_gpioA_buffer_need_sync = FALSE;
	is_gpioB_buffer_need_sync = FALSE;

	// Set IODIRA and IODIRB as output ports:

	uint8_t configBuff[2];

	configBuff[0] = MCP23017_REG_IODIRA; // Register address
	configBuff[1] = MCP23017_IODIR_ALL_OUTPUT; // Value
	HAL_I2C_Master_Transmit(hi2c_MCP23017, MCP23017_ADDRESS, configBuff, 2, 100); // blocking call

	configBuff[0] = MCP23017_REG_IODIRB;
	HAL_I2C_Master_Transmit(hi2c_MCP23017, MCP23017_ADDRESS, configBuff, 2, 100); // blocking call

	// configure GPIO buffers for later use:
	mcp23017_gpioA_tx_Buff[0] = MCP23017_REG_GPIOA;
	mcp23017_gpioB_tx_Buff[0] = MCP23017_REG_GPIOB;

	// init DMA transfer for DMA1 Channel2 (I2C3): (interrupts disabled as they are useless since we don't keep track of when the transfer is complete)
	__HAL_DMA_DISABLE(hdma_MCP23017_tx);
    hdma_MCP23017_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF2; // clear interrupt flags
    hdma_MCP23017_tx->Instance->CNDTR = 2; /* Configure DMA Channel data length */
    hdma_MCP23017_tx->Instance->CPAR = (uint32_t)&(hi2c_MCP23017->Instance->TXDR);
    __HAL_DMA_DISABLE_IT(hdma_MCP23017_tx, DMA_IT_HT | DMA_IT_TC | DMA_IT_TE); // no IT

}


/**
 * Change the state of the given port/pin in the appropriate mcp23017_gpioA/B_tx_Buff[1] GPIO buffer
 * and mark buffer as needing sync to MCP device.
 *
 * This doesn't transmit to the device: for this you need to call mcp23017_Tx_GpioABuffer_Dma() or mcp23017_Tx_GpioBBuffer_Dma().
 *
 * @param PinState GPIO_PIN_RESET or GPIO_PIN_SET
 * @param port MCP23017_GPIOPORT_A or MCP23017_GPIOPORT_B
 * @param pin pin mask, i.e., GPIO_PIN_0 to GPIO_PIN_7
 */
void mcp23017_Write_Pin(MCP23017_GPIO_Port port, uint16_t pin, GPIO_PinState pinState){

	if (port == MCP23017_GPIOPORT_A){

		is_gpioA_buffer_need_sync = FALSE; // lock

		if (pinState != GPIO_PIN_RESET){ // raise pin

			mcp23017_gpioA_tx_Buff[1] |= pin;

		}
		else {

			mcp23017_gpioA_tx_Buff[1] &= ~pin;
		}
		is_gpioA_buffer_need_sync = TRUE; // unlock

	}

	else if (port == MCP23017_GPIOPORT_B){

		is_gpioB_buffer_need_sync = FALSE; // lock

		if (pinState != GPIO_PIN_RESET){

			mcp23017_gpioB_tx_Buff[1] |= pin;

		}
		else {

			mcp23017_gpioB_tx_Buff[1] &= ~pin;

		}
		is_gpioB_buffer_need_sync = TRUE; // unlock
	}
}

/*
 * Set  MCP23017_VCF_2NDORDER_Pin and reset MCP23017_VCF_4THORDER_Pin
 */
void mcp23017_Set_Vcf_2ndOrder(){
	is_gpioA_buffer_need_sync = FALSE; // lock
	mcp23017_gpioA_tx_Buff[1] &= ~MCP23017_VCF_4THORDER_Pin;
	mcp23017_gpioA_tx_Buff[1] |= MCP23017_VCF_2NDORDER_Pin;
	is_gpioA_buffer_need_sync = TRUE; // unlock
}

/*
 * Reset  MCP23017_VCF_2NDORDER_Pin and set MCP23017_VCF_4THORDER_Pin
 */
void mcp23017_Set_Vcf_4thOrder(){
	is_gpioA_buffer_need_sync = FALSE; // lock
	mcp23017_gpioA_tx_Buff[1] &= ~MCP23017_VCF_2NDORDER_Pin;
	mcp23017_gpioA_tx_Buff[1] |= MCP23017_VCF_4THORDER_Pin;
	is_gpioA_buffer_need_sync = TRUE; // unlock
}

/*
 * Raise the SAW pin only
 */
void mcp23017_Set_Vco_Saw(){
	is_gpioA_buffer_need_sync = FALSE; // lock
	mcp23017_gpioA_tx_Buff[1] |= MCP23017_SAW_3340_Pin;
	mcp23017_gpioA_tx_Buff[1] &= ~(MCP23017_PULSE_3340_Pin | MCP23017_TRI_3340_Pin);
	is_gpioA_buffer_need_sync = TRUE; // unlock
}

/*
 * Raise the TRI pin only
 */
void mcp23017_Set_Vco_Tri(){
	is_gpioA_buffer_need_sync = FALSE; // lock
	mcp23017_gpioA_tx_Buff[1] |= MCP23017_TRI_3340_Pin;
	mcp23017_gpioA_tx_Buff[1] &= ~(MCP23017_PULSE_3340_Pin | MCP23017_SAW_3340_Pin);
	is_gpioA_buffer_need_sync = TRUE; // unlock
}

/*
 * Raise the PULSE pin only
 */
void mcp23017_Set_Vco_Pulse(){
	is_gpioA_buffer_need_sync = FALSE; // lock
	mcp23017_gpioA_tx_Buff[1] |= MCP23017_PULSE_3340_Pin;
	mcp23017_gpioA_tx_Buff[1] &= ~(MCP23017_SAW_3340_Pin | MCP23017_TRI_3340_Pin);
	is_gpioA_buffer_need_sync = TRUE; // unlock
}



// from https://github.com/ruda/mcp23017/blob/master/src/mcp23017.c
/*
HAL_StatusTypeDef mcp23017_write(uint16_t reg, uint8_t *data){

	return HAL_I2C_Mem_Write(hi2c_MCP23017, MCP23017_ADDRESS, reg, 1, data, 1, 10);

}*/

