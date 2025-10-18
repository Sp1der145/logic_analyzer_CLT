/*
 * SPIfunctions.c
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */


#include <stdint.h>
#include "stm32l4xx.h"			// General library functions
#include "stm32l4xx_ll_gpio.h"	// GPIO library functions
#include "SoftwareSPI.h"

#define T_DELAY 1000 // Transmission delay



void sendSPI_DOGS(uint8_t data, uint8_t type){

	// PA1 (A1) is the A0 pin that designates commands for data (0 for commands, 1 for data)
	if (type == 0){
		GPIOA->ODR &= ~(1<<1); // Bring low for command
	}
	else{
		GPIOA->ODR |= (1<<1); // Bring high for data
	}



	uint8_t MSN = (data >> 4) & 0x0F;	// Most significant nibble
	uint8_t LSN = data & 0x0F;		 	// Least significant nibble


	GPIOA->ODR &= ~(1<<4); // PA4 (A3) is chip select pin (drop low for start of transmission)

	// First part of the 24 bit transmission 0b00011111 (LSB First)
	sendSPI(0x1F);
	sendSPI(MSN);
	sendSPI(LSN);

	GPIOA->ODR |= (1<<4); // PA4 is chip select pin (bring high for end of transmission)
							// How to synchronise with the end of the spi transmission?

}



void init_display(){


	GPIOA->ODR |= (1<<3); // PA3 (A2) is RST (Reset) pin (Active low, start high to make a pulse)

	// ~2ms delay to allow stable high level before pulse
	for (uint32_t i = 0; i < 10000; i++){}

	GPIOA->ODR &= ~(1<<3); // Drop low for a few ms to reset the display before init

	// ~2ms delay pulse width
	for (uint32_t i = 0; i < 10000; i++){}

	GPIOA->ODR |= (1<<3); // bring back to high to finish the pulse. Stay high indefinitely

	// ~2ms delay to wait after reset before init commands
	for (uint32_t i = 0; i < 10000; i++){}

	sendSPI_DOGS(0x3A, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x09, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x06, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x1E, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x39, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x1B, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x6C, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x56, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x7A, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x38, 0);
	for (uint16_t i = 0; i < T_DELAY; i++){}
	sendSPI_DOGS(0x0F, 0);

}



