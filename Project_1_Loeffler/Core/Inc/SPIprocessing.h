/*
 * SPIprocessing.h
 *
 *  Created on: Oct 17, 2025
 *      Author: trloeffler20
 */

#ifndef SRC_SPIPROCESSING_H_
#define SRC_SPIPROCESSING_H_



// FSM output function
enum OUTPUT getFSMoutput (uint8_t input, struct CompleteFSM *x);

// CODEGDB testing and debug functions. Not needed in final task.
void SPI_data_gen(uint8_t CLK[ARR_SIZE], uint8_t MOSI[ARR_SIZE], uint8_t data);
void SPI_data_print(uint8_t CLK[ARR_SIZE], uint8_t MOSI[ARR_SIZE]);
void PrintBinData(uint8_t arr[SPI_BYTE_LENGTH]);

// Turning SPI (CLK, MOSI) array an array of 8 bits of data received
void acquire_SPI_data(uint8_t CLK[ARR_SIZE], uint8_t MOSI[ARR_SIZE], uint8_t SPIbyte[SPI_BYTE_LENGTH]);

// Data array to hex value
uint8_t ARR2HEX (uint8_t arr[SPI_BYTE_LENGTH]);

// Maybe what the task will look like
void sample_sequence();




#endif /* SRC_SPIPROCESSING_H_ */
