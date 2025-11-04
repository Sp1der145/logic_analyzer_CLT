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

void displayOn(){
	command(0xAF);
	return;
}

void displayOff(){
	command(0xAE);
	return;
}

void softSPI(uint8_t data){
	uint8_t shift;
	for(int8_t i = 7; i>=0; i--){
		shift = (!!(data&(1<<i)));
		if(shift){
			GPIOA -> ODR |= (1<<GMOSI);
		}
		else{
			GPIOA -> ODR &= ~(1<<GMOSI);
		}

		for(volatile int i=0; i<50; i++);
		GPIOA -> ODR |= (1<<GCLK);
		for(volatile int i=0; i<50; i++);
		GPIOA -> ODR &= ~(1<<GCLK);
	}
}

void sendData(uint8_t byte, bool is_data) {
    if (is_data){
    	GPIOB -> ODR |= (1<<GCD);
    }
    else{
    	GPIOB -> ODR &= ~(1<<GCD);
    }
    GPIOB -> ODR &= ~(1<<GCS);
	softSPI(byte);
    GPIOB -> ODR |= (1<<GCS);
	for(volatile uint32_t i=0; i<100; i++);
}

void command(uint8_t cmd) {
	sendData(cmd, false);
}

void giveData(uint8_t data) {
	sendData(data, true);
}

void reset(void) {
	GPIOA -> ODR &= ~(1<<GRESET);
	for(volatile uint32_t i=0; i<10000; i++);
    GPIOA -> ODR |= (1<<GRESET);
	for(volatile uint32_t i=0; i<10000; i++);
}

void lcd_init(void) {
	reset();

	command(0xAE);
    command(0x40); // Set display start line = 0
    command(0xA1); // Reverse view
    command(0xC0); // COM mode
    command(0xA4); // All Pixels On
    command(0xA6); // Display Inverse Off
    command(0xA2); // Bias 1/9
    command(0x2F); // Power control
    command(0x27); // Resistor ratio
    command(0x81); // Set contrast
    command(0x10); // Contrast value
    command(0xFA); // Temperature curve
    command(0x90); // Temperature curve
    displayClear();
	command(0xAF);
}

void displayClear() {
    for(int page = 0; page < 8; page++){
        command(0xB0 | page);                      // Set page
        for(int col = 0; col < 132; col++){
            command(0x10 | ((col >> 4) & 0x0F));
            command(0x00 | (col & 0x0F));
            giveData(0x00);                        // Clear each byte
        }
    }
}


// ----------------- Draw one pixel ----------------------
void draw_pixel(uint8_t x, uint8_t y) {
    uint8_t page = y / 8;
    uint8_t bit  = y % 8;
    uint8_t data = (1 << bit);

    command(0xB0 | page);               // Set page
    command(0x10 | ((x >> 4) & 0x0F));  // Correct MSB
    command(0x00 | (x & 0x0F));         // LSB
    giveData(data);                      // Send the byte
}

