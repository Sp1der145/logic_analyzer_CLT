/*
 * queue.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lmgault22
 */


#include "queue.h"
#include "main.h"


Queue q1; //Character Display Queue





void init_queue(Queue* this){
	this->head = 0;
	(*this).load = 0;

	// Added by thomas
	this->buffer[QUEUE_DEPTH]   = 0xBAADF00D;// Third to last element in the buffer
	this->buffer[QUEUE_DEPTH+1] = 0xBAADF00D;// Second to Last element in the buffer
	this->buffer[QUEUE_DEPTH+2] = 0xBAADF00D;// Last element in the buffer



}


bool enqueue(Queue* this, int32_t data){

	int16_t location;


	if(this->load >= QUEUE_DEPTH){
		return false;
	}

	uint32_t irqs = __get_PRIMASK();
	if(irqs==0){
		__disable_irq();
	}

	// ADDED THIS
	// Adjusting that location if it wraps around the end of the buffer
	if (this->head + this->load >= QUEUE_DEPTH){
		// Wrap around
		location = (this->head + this->load) - QUEUE_DEPTH;
	}
	else {
		// No wrap around
		location = this->head + this->load;
	}

	// AND EDITED THIS
	this->buffer[location] = data;
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
	if(this->load == 0){
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
