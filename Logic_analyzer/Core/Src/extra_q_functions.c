/*
 * extra_q_functions.c
 *
 *  Created on: Nov 4, 2025
 *      Author: trloeffler20
 */

#include "main.h"
#include "queue.h"

extern uint8_t SPI_RX_hex;
extern bool IT_SPI_FLAG;
extern Queue q1;

uint8_t test_array[20];
uint8_t test_index = 0;

void monitor_IT_flags(){
	int32_t hex;


	if (IT_SPI_FLAG){
		test_array[test_index] = SPI_RX_hex;
		test_index++;

		if(test_index > 12){
			test_index =0;
		}


		hex = (int32_t)(SPI_RX_hex) | 0x08000000;
		enqueue(&q1, hex);
		IT_SPI_FLAG = false;
	}

	return;

}

void monitor_queues(){
	int32_t error = 0x00000000;

	for (volatile uint8_t i = 0; i<=2; i++){
		if(q1.buffer[QUEUE_DEPTH+i] != 0xBAADF00D){
			error = 0x0FFF0000;
			enqueue(&q1, error);
		}

	}
}
