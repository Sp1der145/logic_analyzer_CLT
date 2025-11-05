/*
 * DOGS_display_functions.c
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#include "main.h"
#include "queue.h"
#include "SoftwareSPI.h"
#include "DOGS_character_lookup.h"

extern Queue M1_setup_q;
extern Queue M1_data_q;
extern Queue M2_q;
extern Queue SPI_q;






void clear_display(){

	// In order to cycle through, each line has 4 extra "ghost spots", meaning 16 real spots + 4 to fill a line, 80 for 4 rows
	for (uint8_t i = 0; i < 40; i++){
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




void setup_display(uint8_t mode){

	clear_display();

	if (mode == 1){				// Mode 1 = Traces
		set_cursor(1,4);		// Display:
		send_charDOGS('S');		//	Sample rate (us)
		send_charDOGS('R');		//	Trigger mode (1R = Trace 1 rising 1F = Trace 1 falling, 2R, 2F, 3R, 3F)
		send_charDOGS(' ');
		send_charDOGS('u');
		send_charDOGS('s');
		send_charDOGS(':');

		set_cursor(2,2);
		send_charDOGS('T');
		send_charDOGS('r');
		send_charDOGS('i');
		send_charDOGS('g');
		send_charDOGS(' ');
		send_charDOGS('m');
		send_charDOGS('o');
		send_charDOGS('d');
		send_charDOGS('e');
		send_charDOGS(':');
	}


	if (mode == 2){				// Mode 2 = SPI
		set_cursor(1,3);		// Display:
		send_charDOGS('C');		//	CS width (us)
		send_charDOGS('S');		//	SPI data (MSG
		send_charDOGS(' ');
		send_charDOGS('u');
		send_charDOGS('s');
		send_charDOGS(':');

		set_cursor(2,5);
		send_charDOGS('M');
		send_charDOGS('S');
		send_charDOGS('G');
		send_charDOGS(':');
		send_charDOGS(' ');
		send_charDOGS(0);
		send_charDOGS('x');
	}


	// moving the cursor off the screen
	set_cursor(2,18);

}


void init_display(){


	init_queue(&M2_q); // Initialize the Character Display queue


	GPIOA->ODR |= (1<<7); // PA7 (A6) is RST (Reset) pin (Active low, start high to make a pulse)

	// ~2ms delay to allow stable high level before pulse
	for (uint32_t i = 0; i < 10000; i++){}

	GPIOA->ODR &= ~(1<<7); // Drop low for a few ms to reset the display before init

	// ~2ms delay pulse width
	for (uint32_t i = 0; i < 10000; i++){}

	GPIOA->ODR |= (1<<7); // bring back to high to finish the pulse. Stay high indefinitely

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

	setup_display(1);
}





void update_period(uint16_t period){


	uint8_t thousands;
	uint8_t hundreds;
	uint8_t tens;
	uint8_t ones;



	// Period calculations
	if (period <= 9999){
		thousands = period/1000;
		hundreds = (period- (thousands*1000)) / 100;
		tens = (period - (hundreds*100) - (thousands*1000)) / 10;
		ones = period - (thousands*1000) - (hundreds*100) - (tens*10);
	}
	else{ // If period is over 9999, display ????
		thousands = '?';
		hundreds = '?';
		tens = '?';
		ones = '?';
	}


	set_cursor(1,11); // Clearing part of the period to prevent any unwanted values staying on the screen
	send_charDOGS(' ');
	send_charDOGS(' ');
	send_charDOGS(' ');
	send_charDOGS(' ');

	set_cursor(1,10);


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


	// moving the cursor off the screen
	set_cursor(2,18);

}


void update_MSG(uint16_t SPIhex){


	uint8_t SPI_LSN;	// Least significant nibble
	uint8_t SPI_MSN;	// Most significant nibble



	// SPIhex is 16 bit so we can send "??" to the display
	if (SPIhex > 0xFF){
			SPI_LSN = '?';
			SPI_MSN = '?';
	}
	else{
		SPI_LSN = SPIhex & 0x0F;
		SPI_MSN = (SPIhex >> 4) & 0x0F;
	}


	set_cursor(2,12);

	send_charDOGS(SPI_MSN);
	send_charDOGS(SPI_LSN);

	// moving the cursor off the screen
	set_cursor(2,18);

}




// Update SR on the character display
void update_SR(uint16_t sample_rate){


	uint8_t thousands;
	uint8_t hundreds;
	uint8_t tens;
	uint8_t ones;



	// Period calculations
	thousands = sample_rate / 1000;
	hundreds = (sample_rate - (thousands*1000)) / 100;
	tens = (sample_rate - (hundreds*100) - (thousands*1000)) / 10;
	ones = sample_rate - (thousands*1000) - (hundreds*100) - (tens*10);


	set_cursor(1,11); // Clearing part of the period to prevent any unwanted values staying on the screen
	send_charDOGS(' ');
	send_charDOGS(' ');
	send_charDOGS(' ');
	send_charDOGS(' ');

	set_cursor(1,11);


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


	// moving the cursor off the screen
	set_cursor(2,18);

}


// Update trigger mode
void update_TM(uint16_t trigger_mode){

	set_cursor(2,13);


	if(trigger_mode == 1){
		send_charDOGS('1');
		send_charDOGS('R');
	}
	else if(trigger_mode == 2){
		send_charDOGS('1');
		send_charDOGS('F');
	}
	else if(trigger_mode == 3){
		send_charDOGS('2');
		send_charDOGS('R');
	}
	else if(trigger_mode == 4){
		send_charDOGS('2');
		send_charDOGS('F');
	}
	else if(trigger_mode == 5){
		send_charDOGS('3');
		send_charDOGS('R');
	}
	else if(trigger_mode == 6){
		send_charDOGS('3');
		send_charDOGS('F');
	}
	else{
		send_charDOGS('?');
		send_charDOGS('?');
	}

	// moving the cursor off the screen
	set_cursor(2,18);

}





void update_char_display(){ // task

	static uint8_t mode = 1;

	int32_t message; // Variable that data will be dequeued to

	uint16_t CS_period;
	uint16_t SPI_data;
	uint16_t sample_rate;
	uint16_t trigger_mode;


	if(!dequeue(&M2_q, &message)){
		return;
	}

	// 0x0FFF in the header means lower 16 bits are mode
	if ((message & 0x0FFF0000) == 0x0FFF0000){
		if ((uint8_t)(message & 0xF) != mode){	// If the mode message is not the same as the current mode, then the mode has changed
			mode = (uint8_t)(message & 0xF); 	// Change the mode to what the message said
			setup_display(mode);				// Change the display since the mode has changed
		}
	}


	// 0x0800 in the header means lower 16 bits are SPI data
	else if ((message & 0x0F000000) == 0x08000000){
		SPI_data = (uint16_t)(message & 0xFF);
		update_MSG(SPI_data);
	}

	// 0x0400 in the header means lower 16 bits are CS period
	else if ((message & 0x0F000000) == 0x04000000){
		CS_period = (uint16_t)(message & 0xFFFF);
		update_period(CS_period);
	}


	// 0x0200 in the header means lower 16 bits are sample rate
	else if ((message & 0x0F000000) == 0x02000000){
		sample_rate = (uint16_t)(message & 0xFF);
		update_SR(sample_rate);
	}

	// 0x0100 in the header means lower 16 bits are trigger mode
	else if ((message & 0x0F000000) == 0x01000000){
		trigger_mode = (uint16_t)(message & 0xFFFF);
		update_period(trigger_mode);
	}


}






