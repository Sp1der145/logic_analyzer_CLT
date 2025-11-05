/*
 * mode1Source.c
 *
 *  Created on: Oct 8, 2025
 *      Author: crupton23
 */

#include "main.h"
#include "mode1Source.h"

/* ------------------------------------------------------ */
/* ---------------------- GLOBAL VARS -------------------- */
/* ------------------------------------------------------ */

// Input vars
volatile uint8_t P_A_in = UNPRESSED;
volatile uint8_t P_B_in = UNPRESSED;
volatile uint8_t C_A_in;
volatile uint8_t C_B_in;

// Queue backing vars (internal)
volatile uint8_t _triggerTrace = 0; //Can be 0-5
volatile uint32_t _sampleRate = 5; //Can be 0-10

// GPIO input snapshots
volatile uint8_t inPA9;
volatile uint8_t inPA10;
volatile uint8_t inPA8;

/* ------------------------------------------------------ */
/* ---------------------- FSM TABLES --------------------- */
/* ------------------------------------------------------ */

// Encoder Edge Detector
const struct qTransition encoderEdgeTransitions[16] = {
    {UP, GPLOW, UP1q, UNPRESSED},
    {UP, GPHIGH, UP, UNPRESSED},
    {UP1q, GPLOW, UP2q, UNPRESSED},
    {UP1q, GPHIGH, UP, UNPRESSED},
    {UP2q, GPLOW, UP3q, UNPRESSED},
    {UP2q, GPHIGH, UP, UNPRESSED},
    {UP3q, GPLOW, DOWN, PRESSED},
    {UP3q, GPHIGH, UP, UNPRESSED},
    {DOWN, GPLOW, DOWN, PRESSED},
    {DOWN, GPHIGH, DW1q, PRESSED},
    {DW1q, GPLOW, DOWN, PRESSED},
    {DW1q, GPHIGH, DW2q, PRESSED},
    {DW2q, GPLOW, DOWN, PRESSED},
    {DW2q, GPHIGH, DW3q, PRESSED},
    {DW3q, GPLOW, DOWN, PRESSED},
    {DW3q, GPHIGH, UP, UNPRESSED},
};

// Button Edge Detector
const struct qTransition pushButton[8] = {
    {PBqUP, PBlow, PBqUPweak, PBUP},
    {PBqUP, PBhigh, PBqUP, PBUP},
    {PBqDOWN, PBlow, PBqDOWN, PBDOWN},
    {PBqDOWN, PBhigh, PBqDOWNweak, PBDOWN},
    {PBqUPweak, PBlow, PBqDOWN, PBPRESS},
    {PBqUPweak, PBhigh, PBqUP, PBUP},
    {PBqDOWNweak, PBlow, PBqDOWN, PBDOWN},
    {PBqDOWNweak, PBhigh, PBqUP, PBRELEASE},
};

// Trigger Cycle FSM (Outputs 0-5)
const struct qTransition triggerCycles[6] = {
    {ptgTop, 1, pgtMiddle, pgtMiddle}, //Out 1
    {pgtMiddle, 1, pgtBottom, pgtBottom}, //Out 2
    {pgtBottom, 1, ngtTop, ngtTop}, //Out 3
    {ngtTop, 1, ngtMiddle, ngtMiddle}, //Out 4
    {ngtMiddle, 1, ngtBottom, ngtBottom}, //Out 5
    {ngtBottom, 1, ptgTop, ptgTop}, //Out 0
};

/* ------------------------------------------------------ */
/* ---------------------- FSM STRUCTS -------------------- */
/* ------------------------------------------------------ */
struct completeFSM EncA = { .q = 1, .numTransitions = 16, .table = encoderEdgeTransitions };
struct completeFSM EncB = { .q = 1, .numTransitions = 16, .table = encoderEdgeTransitions };
struct completeFSM btnInc = { .q = PBqUP, .numTransitions = 8, .table = pushButton };
struct completeFSM btnDec = { .q = PBqUP, .numTransitions = 8, .table = pushButton };
struct completeFSM triggerTraceFSM = { .q = pgtMiddle, .numTransitions = 6, .table = triggerCycles };

/* ------------------------------------------------------ */
/* ---------------------- GETTERS/SETTERS ---------------- */
/* ------------------------------------------------------ */
//THESE SHOULD BE USED/SENT TO LOGAN SO HE CAN GET THE CURRENT SAMPLE RATE AND TRIGGER TRACE
//This means that whenever Logan goes to draw he needs the current triggerTrace and sampleRate of the system
void setTriggerTrace(uint8_t val) { _triggerTrace = val; }
uint8_t getTriggerTrace(void) { return _triggerTrace; }

void setSampleRate(uint8_t val) { _sampleRate = val; }
uint32_t getSampleRate(void) { return _sampleRate; }

/* ------------------------------------------------------ */
/* ---------------------- FSM HELPER FXN ----------------- */
/* ------------------------------------------------------ */

uint8_t getFSMoutput(struct completeFSM *x, uint8_t in) {
    uint8_t out = 0;
    for (int row = 0; row < x->numTransitions; row++) {
        if (x->table[row].old == (x->q) && x->table[row].in == in) {
            x->q = x->table[row].new;
            out = x->table[row].out;
            break;
        }
    }
    return out;
}

/* ------------------------------------------------------ */
/* ---------------------- INPUT READ FXN ----------------- */
/* ------------------------------------------------------ */

static inline void updateInputs(void) {
    inPA9  = (GPIOA->IDR & (1 << 9))  ? 1 : 0;
    inPA10 = (GPIOA->IDR & (1 << 10)) ? 1 : 0;
    inPA8  = (GPIOA->IDR & (1 << 8))  ? 1 : 0;
}

/* ------------------------------------------------------ */
/* ---------------------- FSM TASKS ---------------------- */
/* ------------------------------------------------------ */

void checkChannelA(void) {
    updateInputs();

    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);
//NEED TO FIX IS UPDATING ON IT"S OWN EVEN WHEN NOT MOVING
    if (C_A_in != P_A_in) {
    	// A has changed. Check the state of B to determine direction.
    	if (C_A_in != C_B_in) {
    		_sampleRate--; // Clockwise rotation
    		if (_sampleRate < 0){ _sampleRate = 0;}
    	} else {
    		_sampleRate++; // Counter-clockwise rotation
    		if (_sampleRate > 10){ _sampleRate = 10;}
    	}
    	P_A_in = C_A_in; // Update the previous state
    }
}

void checkChannelB(void) {
    updateInputs();

    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);

    // Ones Check for a change in Channel B (if A hasn't changed)
    if (C_B_in != P_B_in) {
    	// B has changed. Check the state of A to determine direction.
    	if (C_B_in == C_A_in) {
    		_sampleRate--; // Clockwise rotation
    		if (_sampleRate <= 0){ _sampleRate = 0;}
    	} else {
    		_sampleRate++; // Counter-clockwise rotation
    		if (_sampleRate >= 10){ _sampleRate = 10;}
    	}
    	P_B_in = C_B_in; // Update the previous state
    }
}

void nextTriggerTrace(void) {
    updateInputs();

    uint8_t buttonPress = getFSMoutput(&btnInc, inPA8);
    uint8_t newTrace = getFSMoutput(&triggerTraceFSM, buttonPress);
    // Only update when the FSM actually changes (buttonPress == PBPRESS)
    if (buttonPress == 1) {
        setTriggerTrace(newTrace);
    }


}

/* ------------------------------------------------------ */
/* ---------------------- TASK CALLERS ------------------- */
/* ------------------------------------------------------ */

void checkEncoderInputs(void) {
    checkChannelA();
    checkChannelB();
}

void checkButtonFSM(void) {
    nextTriggerTrace();
}
