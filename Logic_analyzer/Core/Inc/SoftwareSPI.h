/*
 * SoftwareSPI.h
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */

#ifndef SRC_SOFTWARESPI_H_
#define SRC_SOFTWARESPI_H_

void sendSPI(uint8_t data);

void sendSPI_ALPHA(uint8_t data, uint8_t type);

// TASK
void SW_SPI();

// To avoid JPL rule of no time wasting loops
#define MICROSECOND_DELAY for (volatile uint8_t j = 0; j < 2; j++){}

#endif /* SRC_SOFTWARESPI_H_ */
