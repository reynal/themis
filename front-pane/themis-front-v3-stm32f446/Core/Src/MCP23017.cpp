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
#include "PushButton.h"
#include "RotaryEncoder.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "stdio.h"
#include "print.h"
#include "misc.h"

MCP23017::MCP23017(I2C_HandleTypeDef *i2c, Address addr, PushButton* pushARoot, RotaryEncoder* encoderARoot, PushButton* pushBRoot, RotaryEncoder* encoderBRoot){
	_hi2c = i2c;
	_address = addr;
	buttonLinkedListA = pushARoot;
	encoderLinkedListA = encoderARoot;
	buttonLinkedListB = pushBRoot;
	encoderLinkedListB = encoderBRoot;
}

MCP23017::~MCP23017() {}


/**
 * Init a MCP23017 device with the given I2C address and I2C bus handler.
 *
 * Usage: mcp.init(&hi2c1, MCP23017::ADDR_101); // plaque nord, adresses 101, 110 et 111
 *
 */
void MCP23017::init(){

	//reset(); now static!

	if (isConnected()==false) {
		printf("MCP23017 0x%02X I2C error!\n", _address);
		Error_Handler();
	}
	else
		printf("MCP23017 0x%02X I2C init OK\n", _address);

	// port A: output (for debugging purpose only)
	//write(IODIRA, IODIR_ALL_OUTPUT);

	// port A: inputs, pull-up, int enabled
	write(GPPUA, 0xFF);
	HAL_Delay(1);
	write(IODIRA, IODIR_ALL_INPUT);
	HAL_Delay(1);
	read(GPIO_A); // clear pending interrupts
	HAL_Delay(1);
	write(GPINTENA, 0xFF);
	HAL_Delay(1);
	read(GPIO_A); // clear pending interrupts
	HAL_Delay(1);

	// port B: inputs, pull-up, int enabled
	write(GPPUB, 0xFF);
	HAL_Delay(1);
	write(IODIRB, IODIR_ALL_INPUT);
	HAL_Delay(1);
	read(GPIO_B); // clear pending interrupts
	HAL_Delay(1);
	write(GPINTENB, 0xFF);
	HAL_Delay(1);
	read(GPIO_B); // clear pending interrupts
	HAL_Delay(1);

	dumpRegisters();

}

/**
 * Assert the RST pin, in effect resetting register values to their default.
 */
/* static*/ void MCP23017::reset(){
	HAL_GPIO_WritePin(MCP_RST_GPIO_Port, MCP_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MCP_RST_GPIO_Port, MCP_RST_Pin, GPIO_PIN_SET);
}

/**
 * Returns true if the device is connected to the I2C bus and properly working.
 */
bool MCP23017::isConnected(){
	if (HAL_I2C_IsDeviceReady(_hi2c, _address, 5, 100) != HAL_OK) {
		return false;
	}
	return true;
}

void MCP23017::dumpRegisters(){
	printf("---------- 0x%02X: PORT A ----------\n",_address);
	printf("IODIRA=%.2X\n",read(IODIRA));
	printf("PullUp A=%.2X\n",read(GPPUA));
	printf("Enable IntOnChange A=%.2X\n",read(GPINTENA));
	printf("Int Flag A=%.2X\n",read(INTFA));
	printf("Int Capture A=%.2X\n",read(INTCAPA));
	printf("---------- 0x%02X: PORT B ----------\n",_address);
	printf("IODIRB=%.2X\n",read(IODIRB));
	printf("PullUp B=%.2X\n",read(GPPUB));
	printf("Enable IntOnChange B=%.2X\n",read(GPINTENB));
	printf("Int Flag B=%.2X\n",read(INTFB));
	printf("Int Capture B=%.2X\n",read(INTCAPB));
}

/* static */ std::string MCP23017::printPin(Pin pin){

	switch (pin){
	case P0: return "0";
	case P1: return "1";
	case P2: return "2";
	case P3: return "3";
	case P4: return "4";
	case P5: return "5";
	case P6: return "6";
	case P7: return "7";
	}
	return "";
}

/* static */ std::string MCP23017::printPort(Port port){

	switch (port){
	case PORT_A: return "PA";
	case PORT_B: return "PB";
	}
	return "";
}

/**
 * Performs an I2C read operation at the given register address.
 * This is an 8-bit wide read.
 */
int MCP23017::read(uint16_t read_reg){
	uint8_t data;
	HAL_StatusTypeDef status=HAL_I2C_Mem_Read(_hi2c, _address, read_reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
	if (status != HAL_OK) {
		printf("MCP23017 0x%02X Read error!\n", _address);
		Error_Handler();
	}
	return data;
}

/**
 * Performs an I2C write operation at the given register address.
 * This is an 8-bit wide write.
 */
void MCP23017::write(uint16_t write_reg, uint8_t data){
	if (HAL_I2C_Mem_Write(_hi2c, _address, write_reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000) != HAL_OK) {
		printf("MCP23017 0x%02X Write error!\n", _address);
		Error_Handler();
	}
}


/**
 * Attach a linked list of controllers to this device port A.
 * Pointers given here are in effect the first element of the linked lists.
 */
/*void MCP23017::attachControllersA(PushButton* pushLinkedListRoot, RotaryEncoder* encoderLinkedListRoot){
	buttonLinkedListA = pushLinkedListRoot;
	encoderLinkedListA = encoderLinkedListRoot;
}*/

/**
 * Attach a linked list of controllers to this device port B.
 * Pointers given are in effect the first element of the linked lists.
 */
/*void MCP23017::attachControllersB(PushButton* pushLinkedListRoot, RotaryEncoder* encoderLinkedListRoot){
	buttonLinkedListB = pushLinkedListRoot;
	encoderLinkedListB = encoderLinkedListRoot;
}*/

/** debugging method */
void MCP23017::printAttachedControllers(){

	PushButton* push;
	RotaryEncoder* encoder;

	printf("PushButtons on Port A:\n");
	push = buttonLinkedListA;
	while (push != NULL){
		push->print();
		push = push->next;
	};

	printf("Encoders on Port A:\n");
	encoder = encoderLinkedListA;
	while (encoder != NULL){
		encoder->print();
		encoder = encoder->next;
	};

	printf("PushButtons on Port B:\n");
	push = buttonLinkedListB;
	while (push != NULL){
		push->print();
		push = push->next;
	};

	printf("Encoders on Port B:\n");
	encoder = encoderLinkedListB;
	while (encoder != NULL){
		encoder->print();
		encoder = encoder->next;
	};

}

/**
 * This must be called by the appropriate EXTI callback when the INTA pin gets asserted.
 * IRQ timing : ~800us (with I2C @ 100kHz)
 *
 * A typical sequence when rotating an encoder is made up of around 4 to 8 IRQ calls
 * separated by around 10 to 100ms. Amongst these some are due only to mechanical bounces
 * (even with the RC circuitry... must investigate! TODO)
 */
void MCP23017::interruptACallback(){

	uint8_t flagReg = read(INTFA); // 50us
	uint8_t capReg = read(INTCAPA);

	//printf("INTCAPA=%c%c%c%c.%c%c%c%c\n", BYTE_TO_BINARY(capReg));

	PushButton* b = buttonLinkedListA;
	while (b != NULL){
		if ((flagReg & b->mask) != 0){ // this encoder triggered the interrupt
			b->update(capReg);
		}
		b = b->next;
	};

	RotaryEncoder* re = encoderLinkedListA;
	while (re!= NULL){
		if ((flagReg & re->mask) != 0){ // this encoder triggered the interrupt
			re->update(capReg);
		}
		re = re->next;
	};

}

/**
 * This must be called when the INTB pin gets asserted.
 */
void MCP23017::interruptBCallback(){

	uint8_t flagReg = read(INTFB); // 50us
	uint8_t capReg = read(INTCAPB);

	//printf("INTCAPB=%c%c%c%c.%c%c%c%c\n", BYTE_TO_BINARY(capReg));

	PushButton* controller = buttonLinkedListB;
	while (controller != NULL){
		if ((flagReg & controller->mask) != 0){ // this encoder triggered the interrupt
			controller->update(capReg);
		}
		controller = controller->next;
	};

	RotaryEncoder* re = encoderLinkedListB;
	while (re!= NULL){
		if ((flagReg & re->mask) != 0){ // this encoder triggered the interrupt
			re->update(capReg);
		}
		re = re->next;
	};


}

void MCP23017::test(){

	dumpRegisters();

	while(1){
		//write(GPIO_A, 0xFF);
		//printf("GPIOB=%.2X\n",mcp23017_Read(GPIOB));
		//HAL_Delay(200);
		//write(GPIO_A, 0x00);
		//printf("0\n");
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		//printf("Loop: %d\n", x++); printSerial();

		//int b = read(GPIO_A);
		//printf("GPIO_A=%.2X\n",b);
		//printf("%c%c%c%c%c%c%c%c ", BYTE_TO_BINARY(read(GPIO_A)));
		//printf("GPIO_B=%.2X\n",read(GPIO_B));
		//printf("%c%c%c%c%c%c%c%c\n", BYTE_TO_BINARY(read(GPIO_B)));
		HAL_Delay(500);
		read(GPIO_A); // clear pending interrupts
		read(GPIO_B); // clear pending interrupts
	}
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

