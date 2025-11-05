/*
 * display.h
 *
 *  Created on: Oct 8, 2025
 *      Author: lmgault22
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32l4xx_hal.h"
#include <stdbool.h>

void lcd_init(void);
void reset(void);
void command(uint8_t cmd);
void giveData(uint8_t data);
void sendData(uint8_t byte, bool is_data);
void draw_pixel(uint8_t x, uint8_t y);
void softSPI(uint8_t byte, bool is_data);

void displayOn();
void displayOff();
void displayClear();

#define GRESET 4
//PB4
#define GCLK 5
//PA5
#define GMOSI 6
//PA6
#define GCS 1
// PA1
#define GCD 1
//PB1



#endif /* INC_DISPLAY_H_ */
