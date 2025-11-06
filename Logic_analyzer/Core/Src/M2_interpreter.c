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
extern bool IT_SS_DONE_FLAG;


extern Queue M2_q;




void monitor_IT_flags(){
	int32_t hex1;

	int32_t data1;
	uint32_t count = 0;

	// If the SPI transmission is complete, slap a header on it and enqueue it to the char display queue
	if (IT_SPI_FLAG){
		hex1 = (int32_t)(SPI_RX_hex) | 0x08000000;
		enqueue(&M2_q, hex1);
		IT_SPI_FLAG = false;
	}


	// If the SS signal is done, grab the timer value, slap a header on it and enqueue it
	if (IT_SS_DONE_FLAG){
		count = LL_TIM_GetCounter(TIM6); // Get timer value (1 tick = .5us)
		data1 = ((int32_t)(count/2)) | 0x04000000;
		enqueue(&M2_q, data1);
		IT_SS_DONE_FLAG = false; // Reset the flag
		LL_TIM_SetCounter(TIM6, 0);   // Reset counter to 0
	}


	return;

}

