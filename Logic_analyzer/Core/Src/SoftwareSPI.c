/*
 * SoftwareSPI.c
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */

#include "main.h"
#include "SoftwareSPI.h"

void sendSPI(uint8_t data){
	uint8_t bit;
	// LSB first
	// Clock idle high
	//read on the second/rising edge
	// therefore change on the falling edge

	// PA5 (A4) = SCLK
	// PA6 (A5) = MOSI

	GPIOA -> ODR |= (1<<5); // Make sure clock is high before starting
	!!(data & (1)) ? (GPIOA -> ODR |= (uint32_t)(1<<6)):(GPIOA -> ODR &= (uint32_t)~(1<<6)); // setting the MOSI to the correct value before the clock starts


	for (volatile uint8_t i = 0; i < 8; i++){
		// Determine bit value
		bit = !!(data & (1<<i));
		// Delay (According to ssd1803a data sheet, clock width(high or low time) must be at least 400 ns)
		MICROSECOND_DELAY;
		// Set clock low
		GPIOA -> ODR &= (uint32_t)~(1<<5);
		// Setting the MOSI data value on the falling edge
		bit ? (GPIOA -> ODR |= (uint32_t)(1<<6)):(GPIOA -> ODR &= (uint32_t)~(1<<6));
		// Delay
		MICROSECOND_DELAY;
		// Set the clock high
		GPIOA -> ODR |= (1<<5);


	}


}




void sendSPI_DOGS(uint8_t data, uint8_t type){

	// type designates commands or data (0 for commands, 1 for data)


	uint8_t MSN = (data >> 4) & 0x0F;	// Most significant nibble
	uint8_t LSN = data & 0x0F;		 	// Least significant nibble


	GPIOA->ODR &= ~(1<<4); // PA4 (A3) is chip select pin (drop low for start of transmission)

	// First part of the 24 bit transmission 0b0?011111 (LSB First)
	//											^ RS
	if (type == 0){
		sendSPI(0x1F); // RS = 0 for command
	}
	else{
		sendSPI(0x5F); // RS = 1 for data
	}
	sendSPI(LSN);
	sendSPI(MSN);

	GPIOA->ODR |= (1<<4); // PA4 is chip select pin (bring high for end of transmission)
							// How to synchronise with the end of the SPI transmission?

}







