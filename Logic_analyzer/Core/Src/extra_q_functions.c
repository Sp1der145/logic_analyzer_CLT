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
extern bool IT_SS_FLAG;
extern Queue q1;


int32_t test_value = 0;

void monitor_IT_flags(){
	int32_t hex1;
	int32_t hex2;


	if (IT_SPI_FLAG){
		hex1 = (int32_t)(SPI_RX_hex) | 0x08000000;
		enqueue(&q1, hex1);
		IT_SPI_FLAG = false;
	}






	if (IT_SS_FLAG){
		hex2 = (int32_t)(test_value) | 0x04000000;
		enqueue(&q1, hex2);
		test_value++;
		IT_SS_FLAG = false;
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
