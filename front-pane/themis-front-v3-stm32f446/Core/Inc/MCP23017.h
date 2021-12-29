/*
 * MCP23017.h
 *
 *  Created on: Dec 22, 2021
 *      Author: sydxrey
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
 * NC	11				18	/RESET (default: connected to RPi Pin 37 = GPIO.25 in wPi numbering scheme)
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

#ifndef INC_MCP23017_H_
#define INC_MCP23017_H_

#include "stm32f4xx_hal.h"


//#define USE_DMA

class MCP23017 {

public:
	MCP23017();
	~MCP23017();
	void test();
	void init(I2C_HandleTypeDef *hi2c, uint8_t address); //, RotaryEncoder* rotary_encoder_array);
	void reset();
	bool isConnected();
	void dumpRegisters();
	int read(uint16_t read_reg);
	void write(uint16_t write_reg, uint8_t data);

#ifdef USE_DMA
	void init_Dma();
	void tx_GpioA_Buffer_Dma();
	void tx_GpioB_Buffer_Dma();
	void write_Pin_Dma(MCP23017_GPIO_Port port, uint16_t GPIO_Pin, GPIO_PinState PinState); // GPIO_PIN_RESET or GPIO_PIN_SET
#endif

	/** Port A or B */
	enum Port {
		PORT_A,
		PORT_B
	};

	/** registers: (IOCON.BANK = 0, default) */
	enum Register {
		IODIRA=0x00,
		IODIRB=0x01,
		IPOLA=0x02,
		IPOLB=0x03,
		GPINTENA=0x04,
		GPINTENB=0x05,
		DEFVALA=0x06,
		DEFVALB=0x07,
		INTCONA=0x08,
		INTCONB=0x09,
		IOCON=0x0A,
//	IOCON			0x0B
		GPPUA=0x0C,
		GPPUB=0x0D,
		INTFA=0x0E,
		INTFB=0x0F,
		INTCAPA=0x10,
		INTCAPB=0x11,
 		GPIO_A=0x12, // TODO : why does GPIOA trigger an error?
		GPIO_B=0x13,
		OLATA=0x14,
		OLATB=0x15
	};

	/** useful register values */
	enum Value {
		IODIR_ALL_OUTPUT=0x00,
		IODIR_ALL_INPUT=0xFF
	};

	/** device adress: ADR_A2A1A0 */
	enum Address {
		ADDR_000=0x40, // = 0x20 shifted one bit to the left (000)
		ADDR_001=0x42, // 001
		ADDR_002=0x44, // 010
		ADDR_003=0x46, // 011
		ADDR_004=0x48, // 100
		ADDR_005=0x4A, // 101
		ADDR_006=0x4C, // 110
		ADDR_007=0x4E  // 111
	};

	enum PinDirection {
		OUTPUT,
		INPUT
	};

	/**
	 * pin 8 bit masks
	 */
	enum Pin {

		P0=0x01,
		P1=0x02,
		P2=0x04,
		P3=0x08,
		P4=0x10,
		P5=0x20,
		P6=0x40,
		P7=0x80
	};

	/**
	 * available parameters for the IOCON register
	 */
	enum IOCONFields{

		/** Controls how the registers are addressed
		 * 1 = The registers associated with each port are separated into different banks.
		 * 0 = The registers are in the same bank (addresses are sequential). */
		BANK=0x80,

		/** INT Pins Mirror bit
		 * 1 = The INT pins are internally connected
		 * 0 = The INT pins are not connected. INTA is associated with PORTA and INTB is associated with PORTB */
		MIRROR=0x40,

		/** Sequential Operation mode bit
		 * 1 = Sequential operation disabled, address pointer does not increment.
		 * 0 = Sequential operation enabled, address pointer increments. */
		SEQOP=0x20,

		/** Slew Rate control bit for SDA output
		 * 1 = Slew rate disabled
		 * 0 = Slew rate enabled */
		DISSLW0x10,

		// not implemented: HAEN (SPI version only)

		/** Configures the INT pin as an open-drain output
		 * 1 = Open-drain output (overrides the INTPOL bit.)
		 * 0 = Active driver output (INTPOL bit sets the polarity.) */
		ODR=0x04,

		/** This bit sets the polarity of the INT output pin
		 * 1 = Active-high
		 * 0 = Active-low*/
		INTPOL=0x02
	};


private:

	I2C_HandleTypeDef *_hi2c;
	uint8_t _address;

#ifdef MCP23017_USE_DMA
	uint8_t mcp23017_gpioA_tx_Buff[2]; // {GPIOA register, value}
	uint8_t mcp23017_gpioB_tx_Buff[2]; // {GPIOB register, value}
	Boolean is_gpioA_buffer_need_sync; // set to false as soon as mcp23017_gpioA_tx_Buff[1] is being written by another thread
	Boolean is_gpioB_buffer_need_sync;
#endif

};



#endif /* INC_MCP23017_H_ */
