/*
 * queue.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lmgault22
 */


////////////////////////////////////////////////////////////////////////////////
//								INCLUDE FILES								  //
////////////////////////////////////////////////////////////////////////////////


#include "queue.h"
#include "main.h"


////////////////////////////////////////////////////////////////////////////////
//							  QUEUE INSTANCES								  //
////////////////////////////////////////////////////////////////////////////////

Queue M1_setup_q;	// Mode1 Setup Queue
Queue M1_data_q;	// Mode1 Data Queue
Queue M2_q;			// Mode2 Queue
Queue SPI_q;		// SPI Queue



////////////////////////////////////////////////////////////////////////////////
//									FUNCTIONS								  //
////////////////////////////////////////////////////////////////////////////////


// Initializing the Queue with 0xBAADFOOD at the end of the buffer
void init_queue(Queue* this){
	this->head = 0;
	this->load = 0;

	this->buffer[QUEUE_DEPTH]   = 0xBAADF00D; // Third to last element in the buffer
	this->buffer[QUEUE_DEPTH+1] = 0xBAADF00D; // Second to Last element in the buffer
	this->buffer[QUEUE_DEPTH+2] = 0xBAADF00D; // Last element in the buffer

}

// Enqueueing function
bool enqueue(Queue* this, int32_t data){

	int16_t location;


	if(this->load >= QUEUE_DEPTH){
		return false;
	}

	uint32_t irqs = __get_PRIMASK();
	if(irqs==0){
		__disable_irq();
	}


	// Adjusting that location if it wraps around the end of the buffer
	if (this->head + this->load >= QUEUE_DEPTH){
		// Wrap around
		location = (this->head + this->load) - QUEUE_DEPTH;
	}
	else {
		// No wrap around
		location = this->head + this->load;
	}


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

// Dequeueing function
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



// This function monitors all of the queues to ensure there is no buffer overflow by checking all of the bad food values
// the Queues get reinitialized if there is corruption
void monitor_queues(){


	// Iterating through the last 3 ("hidden") values of the M1_setup_q queue buffer
	for (volatile uint8_t i = 0; i<=2; i++){
		if(M1_setup_q.buffer[QUEUE_DEPTH+i] != 0xBAADF00D){
			init_queue(&M1_setup_q);
		}
	}


	// Iterating through the last 3 ("hidden") values of the M1_data_q queue buffer
	for (volatile uint8_t i = 0; i<=2; i++){
		if(M1_data_q.buffer[QUEUE_DEPTH+i] != 0xBAADF00D){
			init_queue(&M1_data_q);
		}
	}


	// Iterating through the last 3 ("hidden") values of the M2_q queue buffer
	for (volatile uint8_t i = 0; i<=2; i++){
		if(M2_q.buffer[QUEUE_DEPTH+i] != 0xBAADF00D){
			init_queue(&M2_q);
		}
	}


	// Iterating through the last 3 ("hidden") values of the SPI_q queue buffer
	for (volatile uint8_t i = 0; i<=2; i++){
		if(SPI_q.buffer[QUEUE_DEPTH+i] != 0xBAADF00D){
			init_queue(&SPI_q);
		}
	}

}

// Simple function that inits all the queues
void init_all_queues(){

	init_queue(&M1_setup_q);	// Mode1 Setup Queue
	init_queue(&M1_data_q);		// Mode1 Data Queue
	init_queue(&M2_q);			// Mode2 Queue
	init_queue(&SPI_q);			// SPI Queue

	return;
}



