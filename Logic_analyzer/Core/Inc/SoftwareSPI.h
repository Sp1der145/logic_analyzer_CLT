/*
 * SoftwareSPI.h
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */

#ifndef SRC_SOFTWARESPI_H_
#define SRC_SOFTWARESPI_H_

// To avoid JPL rule of no time wasting loops
#define MICROSECOND_DELAY for (volatile uint8_t j = 0; j < 1; j++){}

// TASK
void SW_SPI();


void softSPI_LSB(uint8_t data);

void softSPI_MSB(uint8_t data, bool is_data);


void sendSPI_ALPHA(uint8_t data, bool is_data);

void sendSPI_GRAPH(uint8_t byte, bool is_data);




#endif /* SRC_SOFTWARESPI_H_ */
