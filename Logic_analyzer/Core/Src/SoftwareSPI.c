/*
 * SoftwareSPI.c
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */

#include "main.h"
#include "SoftwareSPI.h"
#include "queue.h"

extern Queue SPI_q;


// TASK
void SW_SPI(){

	int32_t message;
	uint8_t byte;


	// 36 init messages for both displays
	// 4-8 for character display each time
	// 335 for the graphics display
	for(uint16_t i = 0; i<400; i++){

		if (!dequeue(&SPI_q, &message)){
			return;
		}
		byte = (uint8_t)(message & 0xFF);

		if ((message & 0x0F000000) == 0x01000000){ // 1 in the second nibble of the header means the byte if for the graphics display
			if((message & 0x00F00000) == 0x00100000){ // 1 in the third nibble of the header means the SPI transmission is data
				sendSPI_GRAPH(byte, true); // is data
			}
			else{	// is not data
				sendSPI_GRAPH(byte, false);
			}
		}

		if ((message & 0x0F000000) == 0x02000000){ // 2 in the second nibble of the header means the byte if for the alpha-numeric display
			if((message & 0x00F00000) == 0x00100000){ // 1 in the third nibble of the header means the SPI transmission is data
				sendSPI_ALPHA(byte, true); // is data
			}
			else{
				sendSPI_ALPHA(byte, false);	// is not data
			}
		}
	}



	return;
}



void softSPI_LSB(uint8_t data){
	uint8_t bit;
	// LSB first
	// Clock idle high
	//read on the second/rising edge
	// therefore change on the falling edge

	// PA5 (A4) = SCLK
	// PA6 (A5) = MOSI

	GPIOA -> ODR |= (1<<5); // Make sure clock is high before starting
	!!(data & (1)) ? (GPIOA -> ODR |= (uint32_t)(1<<6)):(GPIOA -> ODR &= (uint32_t)~(1<<6)); // setting the MOSI to the correct value before the clock starts


	// Somehow this works with no delay loops, just straight operations, one 24 bit packet at 43us
	for (volatile uint8_t i = 0; i < 8; i++){
		// Determine bit value
		bit = !!(data & (1<<i));
		// Delay (According to ssd1803a data sheet, clock width(high or low time) must be at least 400 ns)
		//MICROSECOND_DELAY;
		// Set clock low
		GPIOA -> ODR &= (uint32_t)~(1<<5);
		// Setting the MOSI data value on the falling edge
		bit ? (GPIOA -> ODR |= (uint32_t)(1<<6)):(GPIOA -> ODR &= (uint32_t)~(1<<6));
		// Delay
		//MICROSECOND_DELAY;
		// Set the clock high
		GPIOA -> ODR |= (1<<5);

	}

	return;
}


void softSPI_MSB(uint8_t data, bool is_data){
	uint8_t shift;

    GPIOA -> ODR &= ~(1<<GCS);
	for(int8_t i = 7; i>=0; i--){
		shift = (!!(data&(1<<i)));
		if(shift){
			GPIOA -> ODR |= (1<<GMOSI);
		}
		else{
			GPIOA -> ODR &= ~(1<<GMOSI);
		}

		for(volatile int i=0; i<50; i++); // does this delay need to be 50?
		GPIOA -> ODR |= (1<<GCLK);
		for(volatile int i=0; i<50; i++);
		GPIOA -> ODR &= ~(1<<GCLK);
	}

    GPIOA -> ODR |= (1<<GCS);

    return;
}




// Specific SPI Protocol for the alpha-numeric (character) display
void sendSPI_ALPHA(uint8_t data, bool is_data){

	// type designates commands or data (0 for commands, 1 for data)


	uint8_t MSN = (data >> 4) & 0x0F;	// Most significant nibble
	uint8_t LSN = data & 0x0F;		 	// Least significant nibble


	GPIOA->ODR &= ~(1<<4); // PA4 (A3) is chip select pin (drop low for start of transmission)

	// First part of the 24 bit transmission 0b0?011111 (LSB First)
	//											^ RS
	if (is_data){
		softSPI_LSB(0x5F); // RS = 1 for data
	}
	else{
		softSPI_LSB(0x1F); // RS = 0 for command
	}
	softSPI_LSB(LSN);
	softSPI_LSB(MSN);

	GPIOA->ODR |= (1<<4); // PA4 is chip select pin (bring high for end of transmission)

	return;
}





void sendSPI_GRAPH(uint8_t byte, bool is_data){
    if (is_data){
    	GPIOB -> ODR |= (1<<GCD);
    }
    else{
    	GPIOB -> ODR &= ~(1<<GCD);
    }

	softSPI_MSB(byte, is_data);

	//for(volatile uint32_t i=0; i<100; i++); //For debugging pleasure

	return;
}












