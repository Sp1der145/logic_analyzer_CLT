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

void sendData(uint8_t byte, bool is_data) {
    if (is_data){
    	GPIOB -> ODR |= (1<<1);
    }
    else{
    	GPIOB -> ODR &= ~(1<<1);
    }

    GPIOB -> ODR &= ~(1<<0);
    GPIOB -> ODR |= (1<<0);
}

void command(uint8_t cmd) {
	sendData(cmd, false);
}

void giveData(uint8_t data) {
	sendData(data, true);
}

void reset(void) {
	GPIOA -> ODR &= ~(1<<12);
    HAL_Delay(10);
    GPIOA -> ODR |= (1<<12);
    HAL_Delay(10);
}

void lcd_init(void) {
	reset();

    displayOff();
    command(0x40); // Set display start line = 0
    command(0xA2); // LCD Bias 1/9
    command(0xC0); // COM mode
    command(0x27); // Resistor ratio
    command(0x2F); // Power control
    command(0x81); // Set contrast
    command(0x10); // Contrast value
    displayOn();
}

// ----------------- Draw one pixel ----------------------
void draw_pixel(uint8_t x, uint8_t y) {
    uint8_t page = y / 8; //row
    uint8_t bit  = y % 8; //row's row
    uint8_t data = (1 << bit);

    command(0xB0 | page);           // Set page
    command(0x10 | (x & 0xF0 ));    // Column MSB
    command(0x00 | (x & 0x0F));     // Column LSB
    giveData(data);
}
