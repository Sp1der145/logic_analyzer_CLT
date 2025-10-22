/*
 * SoftwareSPI.h
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */

#ifndef INC_SOFTWARESPI_H_
#define INC_SOFTWARESPI_H_

#include <stdint.h>

void sendSPI(uint8_t data);

void sendSPI_DOGS(uint8_t data, uint8_t type);



#endif /* INC_SOFTWARESPI_H_ */
