/*
 * DOGS_display_functions.c
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#include <stdint.h>
#include "stm32l4xx_ll_gpio.h"	// GPIO library functions
#include "SoftwareSPI.h"
#include "DOGS_character_lookup.h"

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
      static const uint8_t row_addr[] = {0x00, 0x20, 0x40, 0x60};
      uint8_t addr = row_addr[row - 1] + (col - 1);
      sendSPI_DOGS(0x80 | addr, 0);  // command set DDRAM address
}


void send_charDOGS(int8_t data){

	uint8_t hex = char_to_DOGShex(data);
	sendSPI_DOGS(hex, 1);

}




void setup_display(){

	clear_display();

	/* For frequency
	set_cursor(1,4);
	send_charDOGS('H');
	send_charDOGS('z');
	send_charDOGS(':');
	*/

	/* For period */
	set_cursor(1,4);
	send_charDOGS('u');
	send_charDOGS('s');
	send_charDOGS(':');


	set_cursor(2,3);
	send_charDOGS('M');
	send_charDOGS('S');
	send_charDOGS('G');
	send_charDOGS(':');
	send_charDOGS(' ');
	send_charDOGS(0);
	send_charDOGS('x');

	// moving the cursor off the screen
	set_cursor(2,18);


}


void update_display_values(uint16_t period, uint16_t SPIhex){

	uint8_t thousands;
	uint8_t hundreds;
	uint8_t tens;
	uint8_t ones;

	uint8_t SPI_LSN;	// Least significant nibble
	uint8_t SPI_MSN;	// Most significant nibble


	// Period calculations
	if (period <= 9999){
		thousands = period/1000;
		hundreds = (period- (thousands*1000)) / 100;
		tens = (period - (hundreds*100) - (thousands*1000)) / 10;
		ones = period - (thousands*1000) - (hundreds*100) - (tens*10);
	}
	else{
		thousands = '?';
		hundreds = '?';
		tens = '?';
		ones = '?';
	}

	// SPIhex is 16 bit so we can send "??" to the display
	if (SPIhex > 0xFF){
			SPI_LSN = '?';
			SPI_MSN = '?';
	}
	else{
		SPI_LSN = SPIhex & 0x0F;
		SPI_MSN = (SPIhex >> 4) & 0x0F;
	}


	set_cursor(1,8);


	if (thousands != 0){
		send_charDOGS(thousands);
	}
	if (hundreds != 0 || thousands != 0){
		send_charDOGS(hundreds);
	}
	if (tens != 0 || hundreds != 0 || thousands != 0){
		send_charDOGS(tens);
	}
	send_charDOGS(ones);


	set_cursor(2,10);

	send_charDOGS(SPI_MSN);
	send_charDOGS(SPI_LSN);



	// moving the cursor off the screen
	set_cursor(2,18);
}






void clear_display_values(){

	set_cursor(1,8);
	send_charDOGS(' ');
	send_charDOGS(' ');
	send_charDOGS(' ');
	send_charDOGS(' ');


	set_cursor(2,10);
	send_charDOGS(' ');
	send_charDOGS(' ');

	// moving the cursor off the screen
	set_cursor(2,18);
}





