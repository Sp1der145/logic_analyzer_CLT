/*
 * DOGS_display_functions.c
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#include <DOGS_character_lookup.h>
#include <stdint.h>
#include <SoftwareSPI.h>
#include "stm32l4xx_ll_gpio.h"	// GPIO library functions







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
	sendSPI_DOGS(0x09, 0); // Display line mode? 09 for normal 1B for double line
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



	/* Set character table ROM A
	sendSPI_DOGS(0x3A, 0);
	sendSPI_DOGS(0x72, 0);
	sendSPI_DOGS(0x0C, 1);
	sendSPI_DOGS(0x38, 0);
	*/

	// Setting the display to
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
      sendSPI_DOGS(0x80 | addr, 0);  // The address/cursor location is 0x80 + row offset + column offset

}

void send_charDOGS(uint8_t data){

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


	uint8_t hundreds;
	uint8_t tens;
	uint8_t ones;

	uint8_t SPI_LSN;	// Least significant nibble
	uint8_t SPI_MSN;	// Most significant nibble


	// Period calculations
	if (period < 10000){
		hundreds = period/100;
		tens = (period - (hundreds*100)) / 10;
		ones = period - (hundreds*100) - (tens*10);
	}
	else{
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

	if (hundreds != 0){
		send_charDOGS(hundreds);
	}
	if (tens != 0 || hundreds != 0){
		send_charDOGS(tens);
	}
	send_charDOGS(ones);


	set_cursor(2,10);

	send_charDOGS(SPI_LSN);
	send_charDOGS(SPI_MSN);



	// moving the cursor off the screen
	//set_cursor(2,18);
}





void screensaver(){
	clear_display();
	for (uint32_t i = 0; i < 1000000; i++){}
	set_cursor(1, 3);
	sendSPI_DOGS(0x54, 1); //T
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x6F, 1); //o
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x6D, 1); //m
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x6D, 1); //m
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x79, 1); //y
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x20, 1); // *space*
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x4A, 1); //J
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x6F, 1); //o
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x68, 1); //h
	for (uint32_t i = 0; i < 1000000; i++){}
	sendSPI_DOGS(0x6E, 1); //n
	for (uint32_t i = 0; i < 5000000; i++){}
}






