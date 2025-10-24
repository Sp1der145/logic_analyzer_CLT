/*
 * DOGS_display_functions.c
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#include <stdint.h>
#include "stm32l4xx_ll_gpio.h"	// GPIO library functions
#include "SoftwareSPI.h"


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
	sendSPI_DOGS(0x09, 0); // Display line mode?
	sendSPI_DOGS(0x06, 0);
	sendSPI_DOGS(0x1E, 0);
	sendSPI_DOGS(0x39, 0);
	sendSPI_DOGS(0x1B, 0);
	sendSPI_DOGS(0x6C, 0);
	sendSPI_DOGS(0x54, 0); //last 2 bits here are to MSB of contrast register (DB1 = C5, DB0 = C4) C5 C4 C3 C2 C1 C0
						   // 0x54 = 0 0 C3 C2 C1 C0
	                       // 0x55 = 0 1 C3 C2 C1 C0
						   // 0x56 = 1 0 C3 C2 C1 C0
		                   // 0x57 = 1 1 C3 C2 C1 C0
	sendSPI_DOGS(0x77, 0); // Set Contrast (DB3-DB0=C3-C0) -> C = 001111
	sendSPI_DOGS(0x38, 0); // RE = 0, IS = 0
	sendSPI_DOGS(0x0F, 0);


	// Set display to 2 lines
	sendSPI_DOGS(0x3A, 0);
	sendSPI_DOGS(0x1B, 0);
	sendSPI_DOGS(0x3C, 0);




}

void clear_display(){

	// In order to cycle through, each line has 4 extra "ghost spots", meaning 16 real spots + 4 to fill a line, 80 for 4 rows
	for (uint8_t i = 0; i < 80; i++){
		  sendSPI_DOGS(0x20, 1); // 0x20 = space (nothing)
	  }
}



void set_cursor(uint8_t row, uint8_t col) {
      static const uint8_t row_addr[] = {0x00, 0x20, 0x40, 0x40};
      uint8_t addr = row_addr[row - 1] + (col - 1);
      sendSPI_DOGS(0x80 | addr, 0);  // command set DDRAM address
}
