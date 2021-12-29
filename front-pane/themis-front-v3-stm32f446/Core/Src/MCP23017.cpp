/*
 * MCP23017.cpp
 *
 *  Created on: Dec 22, 2021
 *      Author: sydxrey
 *
 *
 * MCP23017 device with DMA support
 *
 * GPB0	1	MCP23017	28	GPA7
 * GPB1	2				27	GPA6
 * GPB2	3				26	GPA5
 * GPB3	4				25	GPA4
 * GPB4	5				24	GPA3
 * GPB5	6				23	GPA2
 * GPB6	7				22	GPA1
 * GPB7	8				21	GPA0
 * Vdd	9				20	INTA
 * Vss	10				19	INTB
 * NC	11				18	/RESET
 * SCK	12				17	A2
 * SDA	13				16	A1
 * NC	14				15	A0
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

#include "MCP23017.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "stdio.h"
#include "print.h"

extern char print_buffer[50];

MCP23017::MCP23017(){

}

MCP23017::~MCP23017() {
}

void MCP23017::test(){

	dumpRegisters();

	while(1){
		write(GPIO_A, 0xFF);
		//sprintf(print_buffer, "GPIOB=%.2X\n",mcp23017_Read(GPIOB));
		HAL_Delay(200);
		write(GPIO_A, 0x00);
		//sprintf(print_buffer, "0\n");
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		//sprintf(print_buffer, "Loop: %d\n", x++); printSerial();
		HAL_Delay(200);
	}
}

void MCP23017::init(I2C_HandleTypeDef *hi2c, uint8_t address){ //, RotaryEncoder* rotary_encoder_array) {

	_hi2c = hi2c;
	_address = address;
	//_rotary_encoder_array = rotary_encoder_array;

	reset();

	if (isConnected()==false) Error_Handler();

	// port A: output
	//write(IODIRA, IODIR_ALL_OUTPUT);

	// port A: inputs, pull-up, int enabled
	write(IODIRA, IODIR_ALL_INPUT);
	write(GPPUA, 0xFF);
	write(GPINTENA, 0xFF);
	read(GPIO_A); // clear pending interrupts

	// port B: inputs, pull-up, int enabled
	write(IODIRB, IODIR_ALL_INPUT);
	write(GPPUB, 0xFF);
	write(GPINTENB, 0xFF);
	read(GPIO_B); // clear pending interrupts

	dumpRegisters();

}

void MCP23017::reset(){
	HAL_GPIO_WritePin(MCP_RST_GPIO_Port, MCP_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MCP_RST_GPIO_Port, MCP_RST_Pin, GPIO_PIN_SET);
}

bool MCP23017::isConnected(){
	if (HAL_I2C_IsDeviceReady(_hi2c, _address, 2, 5) != HAL_OK) {
		return false;
	}
	return true;
}

void MCP23017::dumpRegisters(){ // sprintf(print_buffer, "hello %d!\n", 50); printSerial();
	sprintf(print_buffer, "---------- PORT A ----------\n"); printSerial();
	sprintf(print_buffer, "IODIRA=%.2X\n",read(IODIRA)); printSerial();
	sprintf(print_buffer, "PullUp A=%.2X\n",read(GPPUA)); printSerial();
	sprintf(print_buffer, "Enable IntOnChange A=%.2X\n",read(GPINTENA)); printSerial();
	sprintf(print_buffer, "Int Flag A=%.2X\n",read(INTFA)); printSerial();
	sprintf(print_buffer, "Int Capture A=%.2X\n",read(INTCAPA)); printSerial();
	sprintf(print_buffer, "---------- PORT B ----------\n"); printSerial();
	sprintf(print_buffer, "IODIRB=%.2X\n",read(IODIRB)); printSerial();
	sprintf(print_buffer, "PullUp B=%.2X\n",read(GPPUB)); printSerial();
	sprintf(print_buffer, "Enable IntOnChange B=%.2X\n",read(GPINTENB)); printSerial();
	sprintf(print_buffer, "Int Flag B=%.2X\n",read(INTFB)); printSerial();
	sprintf(print_buffer, "Int Capture B=%.2X\n",read(INTCAPB)); printSerial();
}

int MCP23017::read(uint16_t read_reg){
	uint8_t data;
	if (HAL_I2C_Mem_Read(_hi2c, _address, read_reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000) != HAL_OK) Error_Handler();
	return data;
}

void MCP23017::write(uint16_t write_reg, uint8_t data){
	if (HAL_I2C_Mem_Write(_hi2c, _address, write_reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000) != HAL_OK) Error_Handler();
}


// ************************************ DMA *******************************************************

#ifdef MCP23017_USE_DMA

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
void mcp23017_Init_Device_Dma(){

	is_gpioA_buffer_need_sync = FALSE;
	is_gpioB_buffer_need_sync = FALSE;

	// Set IODIRA and IODIRB as output ports:

	uint8_t configBuff[2];

	configBuff[0] = IODIRA; // Register address
	configBuff[1] = MCP23017_IODIR_ALL_OUTPUT; // Value
	HAL_I2C_Master_Transmit(hi2c_MCP23017, MCP23017_ADDRESS, configBuff, 2, 100); // blocking call

	configBuff[0] = IODIRB;
	HAL_I2C_Master_Transmit(hi2c_MCP23017, MCP23017_ADDRESS, configBuff, 2, 100); // blocking call

	// configure GPIO buffers for later use:
	mcp23017_gpioA_tx_Buff[0] = GPIOA;
	mcp23017_gpioB_tx_Buff[0] = GPIOB;

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
void mcp23017_Write_Pin_Dma(MCP23017_GPIO_Port port, uint16_t pin, GPIO_PinState pinState){

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

#endif



// from https://github.com/ruda/mcp23017/blob/master/src/mcp23017.c
/*
HAL_StatusTypeDef mcp23017_write(uint16_t reg, uint8_t *data){

	return HAL_I2C_Mem_Write(hi2c_MCP23017, MCP23017_ADDRESS, reg, 1, data, 1, 10);

}*/

