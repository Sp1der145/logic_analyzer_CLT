/*
 * queue.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lmgault22
 */


#include "queue.h"
#include "main.h"

void init_queue(Queue* this){
	this->head = 0;
	(*this).load = 0;
}


bool enqueue(Queue* this , int32_t data){
	if(this->load >= QUEUE_DEPTH){
		return false;
	}

	uint32_t irqs = __get_PRIMASK();
	if(irqs==0){
		__disable_irq();
	}

	this->buffer[this->load] = data;
	this->load++;

	if(this->load >= QUEUE_DEPTH){
		this->load = 0;
	}

	if(irqs==0){
		__enable_irq();
	}

	return true;
}

bool dequeue(Queue* this, int32_t* data){
	if(this->head == this->load){
		return false;
	}

	uint32_t irqs = __get_PRIMASK();
	if(irqs==0){
		__disable_irq();
	}

	*data = this->buffer[this->head];
	this->head++;


	if(this->head >= QUEUE_DEPTH){
		this->head = 0;
	}

	this->load--;

	if(irqs==0){
		__enable_irq();
	}

	return true;
}
