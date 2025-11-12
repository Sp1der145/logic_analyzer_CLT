/*
 * ALPHA_display_driver.c
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */


////////////////////////////////////////////////////////////////////////////////
//								INCLUDE FILES								  //
////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "ALPHA_char_lookup.h"
#include "queue.h"
#include "SoftwareSPI.h"


////////////////////////////////////////////////////////////////////////////////
//							  ETERNAL VARIABLES								  //
////////////////////////////////////////////////////////////////////////////////

extern Queue M2_q;
extern Queue SPI_q;



////////////////////////////////////////////////////////////////////////////////
//									FUNCTIONS								  //
////////////////////////////////////////////////////////////////////////////////


// This function clears the display by writing ' '(space) to the entire display
void clear_display(){
	// message stays constant in this function because 0x20 is all that gets sent in this function
	const int32_t message = 0x02100020;

	// In order to cycle through, each line has 4 extra "ghost spots", meaning 16 real spots + 4 to fill a line, 80 for 4 rows
	for (uint8_t i = 0; i < 80; i++){
		enqueue(&SPI_q, message);	// 0x20 = space (nothing)
	  }

	return;
}


// This function sets the cursor on the screen so you can write to anywhere, not needing to cycle through the entire screen
void set_cursor(uint8_t row, uint8_t col) {

	const uint8_t row_addr[] = {0x00, 0x20, 0x40, 0x60};

	uint8_t addr = row_addr[row - 1] + (col - 1);

	enqueue(&SPI_q, 0x02000080 | addr);

	return;
}

// This function takes a char (int8_t, NOT uint8_t)
// and uses a lookup table to convert the char to the correct hex data for the display,
// then sends it to be sent out by the SPI driver
void send_charDOGS(int8_t data){

	uint8_t hex = char_to_DOGShex(data); // Function with the lookup table
	int32_t message = 0x02100000 | hex;
	enqueue(&SPI_q, message);

	return;
}



// This function writes the labels to the display depending on the mode
void setup_display(uint8_t mode){

	clear_display();

	if (mode == 1){				// Mode 1 = Traces
		set_cursor(1,4);		// Display:
		send_charDOGS('S');		// Sample rate (us)
		send_charDOGS('R');		// Trigger mode (1R = Trace 1 rising 1F = Trace 1 falling, 2R, 2F, 3R, 3F)
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

	return;
}


// This function initializes the display according to the datasheet
// A reset pulse, sending all the required init commands, and setting the line mode
void init_display(){

	// PA7 (A6) is RST (Reset) pin (Active low, start high to make a pulse)
	GPIOA->ODR |= (1<<7);

	// ~2ms delay to allow stable high level before pulse
	for (uint32_t i = 0; i < 10000; i++){}

	GPIOA->ODR &= ~(1<<7); // Drop low for a few ms to reset the display before init

	// ~2ms delay pulse width
	for (uint32_t i = 0; i < 10000; i++){}

	GPIOA->ODR |= (1<<7); // bring back to high to finish the pulse. Stay high indefinitely

	// ~2ms delay to wait after reset before init commands
	for (uint32_t i = 0; i < 10000; i++){}


	enqueue(&SPI_q, 0x0200003A);
	enqueue(&SPI_q, 0x02000009);
	enqueue(&SPI_q, 0x02000006);
	enqueue(&SPI_q, 0x0200001E);
	enqueue(&SPI_q, 0x02000039);
	enqueue(&SPI_q, 0x0200001B);
	enqueue(&SPI_q, 0x0200006C);
	enqueue(&SPI_q, 0x02000054);	//last 2 bits here are to MSB of contrast register (DB1 = C5, DB0 = C4) C5 C4 C3 C2 C1 C0
									// 0x54 = 0 0 C3 C2 C1 C0
    								// 0x55 = 0 1 C3 C2 C1 C0
									// 0x56 = 1 0 C3 C2 C1 C0
    								// 0x57 = 1 1 C3 C2 C1 C0
	enqueue(&SPI_q, 0x02000077);	// Set Contrast (DB3-DB0=C3-C0) -> C = 001111
	enqueue(&SPI_q, 0x02000038);
	enqueue(&SPI_q, 0x0200000F);

	// Set display to 2 lines
	enqueue(&SPI_q, 0x0200003A);
	enqueue(&SPI_q, 0x0200001B);
	enqueue(&SPI_q, 0x0200003C);



	setup_display(2);

	return;
}




// This function takes an integer and calculates the digits that need to be displayed on the screen
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

	return;
}


// This function takes the hex data to be displayed and splits up the byte so it can be displayed
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

	return;
}




// This function is identical to update_period, taking an integer and calculating the digits to be displayed
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

	return;
}


// This function displays the trigger mode
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

	return;
}




// This is the task to be called by the main function, handling all the messages received in its queue
void update_char_display(){


	// Mode1: GPIO
	// Mode2: SPI
	static uint8_t mode = 1;


	int32_t message; // Variable that data will be dequeued to

	uint16_t CS_period; // Do I need different variables all of these?
	uint16_t SPI_data;
	uint16_t sample_rate;
	uint16_t trigger_mode;


	if(!dequeue(&M2_q, &message)){
		return;
	}


	// Mode 1
	if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5) == 0){
		if (mode != 1){
			mode = 1;
			setup_display(mode);
		}


		// 0x0200 in the header means lower 16 bits are sample rate
		if ((message & 0x0F000000) == 0x02000000){
			sample_rate = (uint16_t)(message & 0xFF);
			update_SR(sample_rate);
		}

		// 0x0100 in the header means lower 16 bits are trigger mode
		else if ((message & 0x0F000000) == 0x01000000){
			trigger_mode = (uint16_t)(message & 0xFFFF);
			update_period(trigger_mode);
		}

	}

	// Mode 2
	if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5) == 1){
		if (mode != 2){
			mode = 2;
			setup_display(mode);
		}


		// Updating SPI Values if in mode 2
		// 0x0800 in the header means lower 16 bits are SPI data
		if ((message & 0x0F000000) == 0x08000000){
			SPI_data = (uint16_t)(message & 0xFF);
			update_MSG(SPI_data);
		}

		// 0x0400 in the header means lower 16 bits are CS period
		else if ((message & 0x0F000000) == 0x04000000){
			CS_period = (uint16_t)(message & 0xFFFF);
			update_period(CS_period);
		}
	}











	return;
}






