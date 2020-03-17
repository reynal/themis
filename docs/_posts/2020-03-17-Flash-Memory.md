---
layout: post
title:  "Saving music calibration data in a flash memory"
crawlertitle: "Saving music calibration data in a flash memory"
summary: "Saving music calibration data in a flash memory"
date:   2020-03-17 17:04:17 +0700
categories: posts
tags: 'themis'
author: Salome Wattiaux
---

Today, the aim is to save music calibration data in a flash memory. EEPROM are very useful to keep data and use them when we actually need them without filling the flash memory of our microprocessor. Moreover, memory in microcontroller are easily lost when we rebuild the program. EEPROM will keep our data in stock even when it is no longer powered. In our case, we will use the EEPROM to keep the music calibration data when we don’t use them in the EEPROM and to be able to load them in the microcontroller when we need them. In order to do that, we will use a STM32L432KC and an EEPROM, in this case will use the 24C04 EEPROM. 

This EEPROM can save up to 4kbit of data in 2 blocks of 256 * 8 bits, that is enough for what we want to do because will save three table of 72 notes coded in 12 bits. We will also use the STM32 Cube IDE. The STM32 and the EEPROM will communicate with each other using the I2C protocol. Therefore we need to use the HAL library to send the data to the EEPROM and receive them from the EEPROM. In order to do that we will need to configure the output of the STM32, write the code to send data and the code to receive data and wire the EEPROM. 

Configuring the STM32’s output

We need to configure two outputs to generate the SCL (Serial Clock Line) and SDA (Serial Data Line) in order to send and receive data with I2C protocol. We should pay attention to configure those outputs as I2C_SCL and I2C_SDA.

Writing the code

To write the code we will use two HAL function : HAL_I2C_Master_Transmit and HAL_I2C_Master_Receive. Both functions take in argument the following parameters :
I2C_HandleTypeDef *hi2c, a structure that contains the configuration information for the specified I2C, 
uint16_t DevAddress, the device 7 bits address value,
uint8_t *pData, the pointer to data buffer,
uint16_t Size, the amount a data to be sent,
uint32_t Timeout, the timeout duration.
Therefore we need the 7 bits address of the EEPROM available in the datasheet, in our case it is 0xA0 for the first block and 0xA2 for the second block of the 24C04. The data to be sent is saved in a table.
We then write two functions saveInEEPROM and loadFromEEPROM. I chose to put the following arguments for these functions :
uint8_t table[],
int table_size.

To test if my code was working I generated a random table with a hundred arguments and I observed thanks to the SFRs view that the data was sent to the EEPROM with the first function and received from the EEPROM with the second. 

Wiring the EEPROM

After having written the code we need to wire the EEPROM and the STM32 together. I used the datasheet to determine the right alimentation value. We will use a 3,3V alimentation provided by the STM32. We also need to connect some of the pins, like E1 and E2 to determine the address of the EEPROM. We also have to connect the PRE pin to the ground. 
We will wire the circuit as in figure 1.

Fig 1. Circuit used to write and read in an EEPROM

To conclude we were able to write and read data in an EEPROM using a STM32L432KC and a I2C protocol. We needed to understand how the HAL function to use them in the most effective way. It was also capital to search for the right value in 24C04’ datasheet. It can be done by someone who know C language and who has some basics programming skills. 
