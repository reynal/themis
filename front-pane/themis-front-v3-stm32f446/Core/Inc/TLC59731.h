/*
 * TLC59731.h
 *
 *  Created on: Nov 18, 2020
 *      Author: sydxrey
 *
 */

#ifndef SRC_TLC59731_H_
#define SRC_TLC59731_H_

#include "stm32f4xx_hal.h"
#include "RGBColor.h"

#define TLC_ZERO 		0x80
#define TLC_ONE 		0xA0
#define TLC_EOS_SZ		4		// number of Tcycles for the EOS
#define TLC_GSLAT_SZ	10		// number of Tcycles for the GSLAT
#define TLC_LED_COUNT	17		// on a 16 ou 17 leds a chaque fois (nord=17, sud=16 et 17)
#define TLC_BUF_SZ 		(TLC_LED_COUNT * (32 + TLC_EOS_SZ) + TLC_GSLAT_SZ)
// TLC_BUF_SZ = 622 bytes for 17 LEDs ~ 5000 bits, which takes around 5ms

#define TLC59731_USE_DMA

// used for accessing the ISR and IFCR DMA regs: (but one can access them directly using DMA1->xxx)
typedef struct {
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_REG;

class TLC59731 {
public:
	TLC59731(SPI_HandleTypeDef *hspi);
	~TLC59731();

	void update(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
	void update(uint8_t led, uint32_t rgb24bits);
	void update(uint8_t led, RGBColor& color);

	void transmitData();

	void test();


private:
	SPI_HandleTypeDef *_hspi;
	uint8_t _spiBuf[TLC_BUF_SZ];
	bool is_DMA_initialized;

	void initSpiBuffer(uint8_t led);

	int getDataAreaIndex(uint8_t led); // return the index of the data area for the given led in the SPI buffer

};

#endif /* SRC_TLC59731_H_ */
