/*
 * display.c
 *
 *  Created on: Oct 8, 2025
 *      Author: lmgault22
 */


/*What do I need?
 *
 * SPI
 * ------
 * Enable
 * Clock
 * Data
 *
 * Task 1:
 * Turn on the display
 *
 * 10/8
 * I think I am getting somewhere
 * I click run and black lines appear for a short time across the display
 * I think I have wired things correctly
 *
 * 10/22
 * I have made lines appear somewhat coherently, but I am not sure how
 * I tried to make a single pixel appear, but that doesn't seem to be working right now. I have spi disabled, so that could be the culprit.
 */

#include "display.h"
#include "SoftwareSPI.h"
#include "queue.h"

extern Queue SPI_q;


void displayOn(){
	//sendSPI_GRAPH(0xAF, false);
	enqueue(&SPI_q, 0x010000AF);

	return;
}

void displayOff(){
	//sendSPI_GRAPH(0xAE, false);
	enqueue(&SPI_q, 0x010000AE);
	return;
}



void reset(void) {
	GPIOB -> ODR &= ~(1<<GRESET);
	for(volatile uint32_t i=0; i<10000; i++);
    GPIOB -> ODR |= (1<<GRESET);
	for(volatile uint32_t i=0; i<10000; i++);
}

void lcd_init(void) {
	reset();

	displayOff();
/*
	sendSPI_GRAPH(0x40, false); // Set display start line = 0
	sendSPI_GRAPH(0xA1, false); // Reverse view
	sendSPI_GRAPH(0xC0, false); // COM mode
	sendSPI_GRAPH(0xA4, false); // All Pixels On
	sendSPI_GRAPH(0xA6, false); // Display Inverse Off
	sendSPI_GRAPH(0xA2, false); // Bias 1/9
	sendSPI_GRAPH(0x2F, false); // Power control
	sendSPI_GRAPH(0x27, false); // Resistor ratio
	sendSPI_GRAPH(0x81, false); // Set contrast
	sendSPI_GRAPH(0x18, false); // Contrast value
	sendSPI_GRAPH(0xFA, false); // Temperature curve
	sendSPI_GRAPH(0x90, false); // Temperature curve
*/
	enqueue(&SPI_q, 0x01000040);
	enqueue(&SPI_q, 0x010000A1);
	enqueue(&SPI_q, 0x010000C0);
	enqueue(&SPI_q, 0x010000A4);
	enqueue(&SPI_q, 0x010000A6);
	enqueue(&SPI_q, 0x010000A2);
	enqueue(&SPI_q, 0x0100002F);
	enqueue(&SPI_q, 0x01000027);
	enqueue(&SPI_q, 0x01000081);
	enqueue(&SPI_q, 0x01000018);
	enqueue(&SPI_q, 0x010000FA);
	enqueue(&SPI_q, 0x01000090);

	//displayClear();

	displayOn();

}

void displayClear() {
    for(int page = 0; page < 8; page++){
    	//sendSPI_GRAPH((0xB0 | page), false);		// Set page
    	enqueue(&SPI_q, (0x010000B0 | page));
        for(int col = 0; col < 132; col++){
        	//sendSPI_GRAPH((0x10 | ((col >> 4) & 0x0F)), false);
        	enqueue(&SPI_q, (0x01000010 | ((col >> 4) & 0x0F)));
        	//sendSPI_GRAPH((0x00 | (col & 0x0F)), false);
        	enqueue(&SPI_q, (0x01000000 | (col & 0x0F)));
        	//sendSPI_GRAPH(0x00, true);				// Clear each byte
        	enqueue(&SPI_q, 0x01100000);				// Clear each byte
        }
    }
}


// ----------------- Draw one pixel ----------------------
void draw_pixel(uint8_t x, uint8_t y) {
    uint8_t page = y / 8;
    uint8_t bit  = y % 8;
    uint8_t data = (1 << bit);

    //sendSPI_GRAPH((0xB0 | page), false);             	// Set page
    enqueue(&SPI_q, (0x010000B0 | page));				// Set page
    //sendSPI_GRAPH((0x10 | ((x >> 4) & 0x0F)), false);  	// Correct MSB
    enqueue(&SPI_q, (0x01000010 | ((x >> 4) & 0x0F)));		// Correct MSB
    //sendSPI_GRAPH((0x00 | (x & 0x0F)), false);        // LSB
    enqueue(&SPI_q, (0x01000000 | (x & 0x0F)));			// LSB
    //sendSPI_GRAPH(data, true);                      	// Send the byte
    enqueue(&SPI_q, (0x01100000 | data));				// Send the byte
}

