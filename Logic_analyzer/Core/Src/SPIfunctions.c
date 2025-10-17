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




void send_SPI_data(uint8_t data){



	uint8_t MSN = (data >> 4) & 0x0F;	// Most significant nibble
	uint8_t LSN = data & 0x0F;		 	// Least significant nibble


	GPIOA->ODR &= (0<<4); // PA4 is chip select pin (drop low for start of transmission)

	// First part of the 24 bit transmission 0b00011111 (LSB First)
	sendSPI(SPI1, 0x1F);
	sendSPI(SPI1, MSN);
	sendSPI(SPI1, LSN);

	GPIOA->ODR |= (1<<4); // PA4 is chip select pin (bring high for end of transmission)
							// How to synchronise with the end of the spi transmission?

}



void init_display(){

	send_SPI_data(0x3A);
	send_SPI_data(0x09);
	send_SPI_data(0x06);
	send_SPI_data(0x1E);
	send_SPI_data(0x39);
	send_SPI_data(0x1B);
	send_SPI_data(0x6C);
	send_SPI_data(0x56);
	send_SPI_data(0x7A);
	send_SPI_data(0x38);
	send_SPI_data(0x0F);

}



