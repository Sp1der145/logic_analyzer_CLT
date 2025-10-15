/*
 * SoftwareSPI.c
 *
 *  Created on: Oct 15, 2025
 *      Author: trloeffler20
 */

#include <stdint.h>
#include "stm32l4xx_ll_gpio.h"	// GPIO library functions

void sendSPI(uint8_t data){
	uint8_t bit;
	// LSB first
	// Clock idle high
	//read on the second/rising edge
	// therefore change on the falling edge

	// PA5 = SCLK
	// PA6 = MOSI

	GPIOA -> ODR |= (1<<5); // Make sure clock is high before starting
	!!(data & (1)) ? (GPIOA -> ODR |= (1<<6)):(GPIOA -> ODR &= ~(1<<6)); // setting the MOSI to the correct value before the clock starts


	for (volatile uint8_t i = 0; i < 8; i++){
		// Determine bit value
		bit = !!(data & (1<<i));
		// Delay
		for (volatile uint8_t j = 0; j < 10; j++){}
		// Set clock low
		GPIOA -> ODR &= ~(1<<5);
		// Setting the MOSI data value on the falling edge
		bit ? (GPIOA -> ODR |= (1<<6)):(GPIOA -> ODR &= ~(1<<6));
		// Delay
		for (volatile uint8_t j = 0; j < 10; j++){}
		// Set the clock high
		GPIOA -> ODR |= (1<<5);


	}


}
