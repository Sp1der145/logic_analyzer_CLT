/*
 * DOGS_display_functions.h
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#ifndef SRC_DOGS_DISPLAY_FUNCTIONS_H_
#define SRC_DOGS_DISPLAY_FUNCTIONS_H_

void init_display();

void clear_display();

void set_cursor(uint8_t row, uint8_t col);

void send_charDOGS(int8_t data);

void setup_display();

void update_period(uint16_t period);

void update_MSG(uint16_t SPIhex);

void update_char_display();

void clear_display_values();


#endif /* SRC_DOGS_DISPLAY_FUNCTIONS_H_ */
