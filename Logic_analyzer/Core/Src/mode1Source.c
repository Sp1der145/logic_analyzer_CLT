/*
 * mode1Source.c
 *
 *  Created on: Oct 8, 2025
 *      Author: crupton23
 */
#include "main.h"
#include "mode1Source.h"
/* ------------------------------------------------------ */
/* ---------------------- Encdr&Btn --------------------- */
/* ------------------------------------------------------ */

/* ---------------------- QUEUE STUFF --------------------- */
//Externs for QUEUES			PUT IN HEADER FILE!!!!
extern uint8_t triggerTrace;    // current state from trigger FSM out
extern uint32_t sampleRate;     // current sample divider N

// Simple var to share safely between files
static volatile uint8_t _triggerTrace = 0;
static volatile uint32_t _sampleRate = 5000; // default sampleRate

// Getter / Setter interface (declare externs in header for other .c files)
void setTriggerTrace(uint8_t val){
	_triggerTrace = val;
}
uint8_t getTriggerTrace(void){
	return _triggerTrace;
}

void setSampleRate(uint32_t val){
	_sampleRate = val;
}
uint32_t getSampleRate(void){
	return _sampleRate;
}


/* ------------------------------------------------------ */
/* ---------------------- FSM STRUCTS ------------------- */
/* ------------------------------------------------------ */
// Struct for State Machine
struct completeFSM {
	volatile uint8_t q;
	const uint8_t numTransitions;
	const struct qTransition *table;
};

// FSM output getter
uint8_t getFSMoutput(struct completeFSM *x, uint8_t in) {
	uint8_t out = 0;
	for (int row = 0; row < x->numTransitions; row++) {
		if (x->table[row].old == (x->q)) {
			if (x->table[row].in == in) {
				x->q = x->table[row].new;
				out = x->table[row].out;
				break;
			}
		}
	}
	return out;
}


/* ------------------------------------------------------ */
/* ---------------------- FXN'S ------------------------- */
/* ------------------------------------------------------ */
// Get the input from Channel A pin for the Encoder
void checkChannelA() {
    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);

    if (C_A_in != P_A_in) {
        uint32_t newRate = getFSMoutput(&sampleRateFSM, (C_A_in != C_B_in) ? PRESSED : UNPRESSED);
        setSampleRate(newRate);
        P_A_in = C_A_in;
    }
}

void checkChannelB() {
    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);

    if (C_B_in != P_B_in) {
        uint32_t newRate = getFSMoutput(&sampleRateFSM, (C_B_in == C_A_in) ? PRESSED : UNPRESSED);
        setSampleRate(newRate);
        P_B_in = C_B_in;
    }
}

void nextTriggerTrace() {
    uint8_t buttonPress = getFSMoutput(&btnInc, inPA8);
    uint8_t newTrace = getFSMoutput(&triggerTraceFSM, buttonPress);
    setTriggerTrace(newTrace);
}

/* ------------------------------------------------------ */
/* ---------------------- TASK CALLERS ------------------- */
/* ------------------------------------------------------ */
// These functions should be called periodically in the main loop

void checkEncoderInputs(void) {
    checkChannelA();
    checkChannelB();
}

void checkButtonFSM(void) {
    nextTriggerTrace();
}

