/*
 * DOGS_display_functions.h
 *
 *  Created on: Oct 20, 2025
 *      Author: trloeffler20
 */

#ifndef SRC_DOGS_DISPLAY_FUNCTIONS_H_
#define SRC_DOGS_DISPLAY_FUNCTIONS_H_

void clear_display();

void set_cursor(uint8_t row, uint8_t col);

void send_charDOGS(int8_t data);

void setup_display(uint8_t mode);

void init_display();

void update_period(uint16_t period);

void update_MSG(uint16_t SPIhex);

void update_SR(uint16_t sample_rate);

void update_TM(uint16_t trigger_mode);

void update_char_display();


#endif /* SRC_DOGS_DISPLAY_FUNCTIONS_H_ */
