/*
 * queue.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lmgault22
 */


#include "queue.h"

void init_queue(Queue* this){
	this->head = 0;
	(*this).load = 0;
}


bool enqueue(Queue* this , int32_t data){
	if(this->load >= QUEUE_DEPTH){
		return false;
	}

	uint32_t irqs = __getPRIMASK();
	if(irqs==0){
		__disable_irq();
	}

	this->buffer[this->load] = data;
	this->load++;

	if(irqs==0){
		__enable_irq();
	}

	return true;
}

bool dequeue(Queue* this, int32_t* data){
	if(this->head == this->load){
		return false;
	}

	uint32_t irqs = __getPRIMASK();
	if(irqs==0){
		__disable_irq();
	}

	*data = this->buffer[this->head];
	this->load--;

	if(irqs==0){
		__enable_irq();
	}

	return true;
}
