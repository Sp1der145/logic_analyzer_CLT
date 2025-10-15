/*
 * mode1Source.c
 *
 *  Created on: Oct 8, 2025
 *      Author: crupton23
 */
#include "main.h"

/* ---------------------- ENUMS --------------------- */
// Enums for Encoder
enum encoderStates {UP, UP1q, UP2q, UP3q, DOWN, DW1q, DW2q, DW3q};
enum encoderOutput {PRESSED, UNPRESSED};
// Enums for button
enum buttonStates{PBqUP, PBqDOWN, PBqUPweak, PBqDOWNweak};
enum buttonOutput{PBUP, PBDOWN, PBRELEASE, PBPRESS};
enum buttonInput{PBlow, PBhigh};

/* ---------------------- INITS --------------------- */
//Input Var Initialization
volatile uint8_t P_A_in = UNPRESSED;
volatile uint8_t P_B_in = UNPRESSED;
volatile uint8_t incCMD = 0; //DO I STILL NEED THIS? PROB NOT?

//GPIO IN A
volatile uint8_t C_A_in;
//GPIO IN B
volatile uint8_t C_B_in;
// Encoder Channel A Input Mask			WILL NEED A DIFF INPUT PIN
volatile uint8_t inPA9 = (GPIOA->IDR & (1 << 9)) == 0;
// Encoder Channel B Input Mask 		WILL NEED A DIFF INPUT PIN
volatile uint8_t inPA10 = (GPIOA->IDR & (1 << 10)) == 0;

//Button input mask		WILL NEED A DIFF INPUT PIN
volatile uint8_t inPA8 = (GPIOA-> IDR &(1<<8)) == 0;


/* ---------------------- Q-TRANSITION TABLES --------------------- */
// q-Transition table set up
struct qTransition {uint8_t old;uint8_t in;uint8_t new;uint8_t out;};

// Encoder Edge Detector
const struct qTransition encoderEdgeTransitions[16] = {
		{ UP, GPLOW, UP1q, UNPRESSED },
		{ UP, GPHIGH, UP, UNPRESSED },
		{ UP1q, GPLOW, UP2q,UNPRESSED },
		{ UP1q, GPHIGH, UP, UNPRESSED },
		{ UP2q, GPLOW, UP3q,UNPRESSED },
		{ UP2q, GPHIGH, UP, UNPRESSED },
		{ UP3q, GPLOW, DOWN, PRESSED },
		{ UP3q, GPHIGH, UP, UNPRESSED },
		{ DOWN, GPLOW, DOWN, PRESSED },
		{ DOWN, GPHIGH, DW1q, PRESSED },
		{ DW1q, GPLOW, DOWN, PRESSED },
		{ DW1q, GPHIGH, DW2q, PRESSED },
		{ DW2q, GPLOW, DOWN, PRESSED },
		{ DW2q, GPHIGH, DW3q, PRESSED },
		{ DW3q, GPLOW, DOWN, PRESSED },
		{ DW3q, GPHIGH, UP, UNPRESSED }, };
// Button Edge Detector
const struct qTransition pushButton[8] = {
		{PBqUP,       PBlow,  PBqUPweak,   PBUP},
		{PBqUP,       PBhigh, PBqUP,       PBUP},
		{PBqDOWN,     PBlow,  PBqDOWN,     PBDOWN},
		{PBqDOWN,     PBhigh, PBqDOWNweak, PBDOWN},
		{PBqUPweak,   PBlow,  PBqDOWN,     PBPRESS},
		{PBqUPweak,   PBhigh, PBqUP,       PBUP},
		{PBqDOWNweak, PBlow,  PBqDOWN,     PBDOWN},
		{PBqDOWNweak, PBhigh, PBqUP,       PBRELEASE},
	};

/* ---------------------- STATE MACHINES --------------------- */
// Struct for State Machine
struct completeFSM {volatile uint8_t q;const uint8_t numTransitions;const struct qTransition *table;};
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


/* ---------------------- FXNS --------------------- */
// Get the input from Channel A pin for the Encoder
void getChannelAInput(){
	if (inPA9 >= 1) {
		inPA9 = 1;
	} else {
		inPA9 = 0;
	}
}
// Get the input from Channel B pin for the Encoder
void getChannelBInput(){
	if (inPA10 >= 1) {
		inPA10 = 1;
	} else {
		inPA10 = 0;
	}
}
// Update State Machine for Channel A
void updateChannelAFSM(){
	C_A_in = getFSMoutput(&EncA, inPA9);
}
// Update State Machine for Channel B
void updateChannelBFSM(){
	C_B_in = getFSMoutput(&EncB, inPA10);
}
// Check for a change in Channel A
void checkChannelA(){
	if (C_A_in != P_A_in) {
		// A has changed. Check the state of B to determine direction.
			if (C_A_in != C_B_in) {
				count--; // Clockwise rotation			CHANGE TO NOT BE COUNT
			} else {
				count++; // Counter-clockwise rotation	CHANGE TO NOT BE COUNT
			}
			P_A_in = C_A_in; // Update the previous state
	}
}
// Check for a change in Channel A
void checkChannelB(){
	if (C_B_in != P_B_in) {
			// B has changed. Check the state of A to determine direction.
			if (C_B_in == C_A_in) {
				count--; // Clockwise rotation			CHANGE TO NOT BE COUNT
			} else {
				count++; // Counter-clockwise rotation	CHANGE TO NOT BE COUNT
			}
			P_B_in = C_B_in; // Update the previous state
	}
}


/* ---------------------- CW/CCW Checker START --------------------- */
// FSM Structs for Encoder pin A & Encoder pin B
	struct completeFSM EncA = { .q = (uint8_t) 1, .numTransitions = 16, .table = encoderEdgeTransitions };
	struct completeFSM EncB = { .q = (uint8_t) 1, .numTransitions = 16, .table = encoderEdgeTransitions };


/* ---------------------- BUTTON CODE --------------------- */
enum bNum{ptgTop, pgtMiddle, pgtBottom, ngtTop, ngtMiddle, ngtBottom};

const struct qTransition triggerCycles[6] = {
		{ptgTop, 	buttonPress,  pgtMiddle, pgtMiddle},//Out 1 	USE A GETTER to have the output gotten
		{pgtMiddle, buttonPress,  pgtBottom, pgtBottom},//Out 2
		{pgtBottom, buttonPress,  ngtTop, 	 ngtTop   },//Out 3
		{ngtTop, 	buttonPress,  ngtMiddle, ngtMiddle},//Out 4	  	SO FUTURE CONNER TALK TO LOGAN
		{ngtMiddle, buttonPress,  ngtBottom, ngtBottom},//Out 5   	CONNECT THESE 2 FUNCTIONALITIES
		{ngtBottom, buttonPress,  ptgTop, 	 ptgTop   },//Out 0
	};

//buttons set up
struct completeFSM btnInc = {.q = (uint8_t) 1, .numTransitions = 8, .table=pushButton};
//Light FSM Setup
struct completeFSM triggerTraceFSM = {.q = (uint8_t) B1, .numTransitions = 8, .table=lightIncDec};


//While Loop Stuff
if (inPA8 >= 1){
	inPA8 = 1;
}

//Update states
incCMD = getFSMoutput(&btnInc, inPA8);

//DONT NEED THIS JUST USE THE SAME CALLING OF FXNS
//Blink lights
if((inPA8 == 1)&&(inPB1 != 1)){
	//go to next trigger trace
	triggerTrace = getFSMoutput(&triggerTraceFSM, buttonPress);
}

