/*
 * DOGS_display_functions.h
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#ifndef INC_DOGS_DISPLAY_FUNCTIONS_H_
#define INC_DOGS_DISPLAY_FUNCTIONS_H_

#include <stdint.h>

void init_display();

void set_cursor(uint8_t row, uint8_t col);

void clear_display();

void setup_display();

void update_display_values(uint8_t period, uint8_t SPIhex);

void send_charDOGS(uint8_t data);

void screensaver();





#endif /* INC_DOGS_DISPLAY_FUNCTIONS_H_ */
