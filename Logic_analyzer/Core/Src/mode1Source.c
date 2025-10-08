/*
 * mode1Source.c
 *
 *  Created on: Oct 8, 2025
 *      Author: crupton23
 */
 

/* ---------------------- ENCODER CODE --------------------- */
/*Enums for Encoder*/
enum encoderStates {
	UP, UP1q, UP2q, UP3q, DOWN, DW1q, DW2q, DW3q
};
enum encoderOutput {
	PRESSED, UNPRESSED
};
enum buttonInput {
	GPLOW, GPHIGH
};
 
volatile uint8_t P_A_in = UNPRESSED;
volatile uint8_t P_B_in = UNPRESSED;

//MIGHT NOT NEED THIS JUST INCLUDING INCASE NEEDED
typedef struct {
	GPIO_TypeDef *port;
	uint32_t pin;
} GpioPin;


//Edge Detector START
struct qTransition {
	uint8_t old;
	uint8_t in;
	uint8_t new;
	uint8_t out;
};

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

// Edge Detector END


// State Machine Checker START

struct completeFSM {
	volatile uint8_t q;
	const uint8_t numTransitions;
	const struct qTransition *table;
};
//state					   //8					//pointer to array pushButton[0]

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

// State Machine Checker END

uint8_t CheckCcw(uint8_t A_in, uint8_t B_in) {

	return (count);
}


// CW/CCW Checker START

	// FSM Structs for
	struct completeFSM EncA = { .q = (uint8_t) 1, .numTransitions = 16, .table =
			encoderEdgeTransitions };

	struct completeFSM EncB = { (uint8_t) 1, 16, encoderEdgeTransitions };


//THESE SHOULD BE CALLED IN WHILE LOOP
	// Ones Channel A Input Mask
	volatile uint8_t inPA9 = (GPIOA->IDR & (1 << 9)) == 0;
	if (inPA9 >= 1) {
		inPA9 = 1;
	} else {
		inPA9 = 0;
	}

// Ones Channel B Input Mask OG
	volatile uint8_t inPA10 = (GPIOA->IDR & (1 << 10)) == 0;
	if (inPA10 >= 1) {
		inPA10 = 1;
	} else {
		inPA10 = 0;
	}


// Ones Update State Machine OG
	volatile uint8_t C_A_in = getFSMoutput(&EncA, inPA9);
	//GPIO IN A
	volatile uint8_t C_B_in = getFSMoutput(&EncB, inPA10);
	//GPIO IN B

	// Ones Check for a change in Channel A
	if (count <= 0){ count = 0;}
	if (count >= 99){ count = 99;}
	if (C_A_in != P_A_in) {
	// A has changed. Check the state of B to determine direction.
		if (C_A_in != C_B_in) {
			count--; // Clockwise rotation
		} else {
			count++; // Counter-clockwise rotation
		}
		P_A_in = C_A_in; // Update the previous state
	}
	// Ones Check for a change in Channel B (if A hasn't changed)
	else if (C_B_in != P_B_in) {
		// B has changed. Check the state of A to determine direction.
		if (C_B_in == C_A_in) {
			count--; // Clockwise rotation
		} else {
			count++; // Counter-clockwise rotation
		}
		P_B_in = C_B_in; // Update the previous state
	}




/* ---------------------- BUTTON CODE --------------------- */
volatile uint8_t incCMD = 0;
volatile uint8_t decCMD = 0;


enum buttonStates{PBqUP, PBqDOWN, PBqUPweak, PBqDOWNweak};
enum buttonOutput{PBUP, PBDOWN, PBRELEASE, PBPRESS};
enum buttonInput{PBlow, PBhigh};

struct qTransition {uint8_t old; uint8_t in; uint8_t new; uint8_t out;};
const struct qTransition pushButton[8] = {
		{PBqUP,       PBlow,  PBqUPweak,   PBUP},
		{PBqUP,       PBhigh, PBqUP,       PBUP},
		{PBqDOWN,     PBlow,  PBqDOWN,     PBDOWN},
		{PBqDOWN,     PBhigh, PBqDOWNweak, PBDOWN}, // 1 = Button Pressed 0 = Button Not Pressed
		{PBqUPweak,   PBlow,  PBqDOWN,     PBPRESS},
		{PBqUPweak,   PBhigh, PBqUP,       PBUP},
		{PBqDOWNweak, PBlow,  PBqDOWN,     PBDOWN},
		{PBqDOWNweak, PBhigh, PBqUP,       PBRELEASE},
	};

enum bNum{ptgTop, pgtMiddle, pgtBottom, ngtTop, ngtMiddle, ngtBottom};

const struct qTransition triggerCycles[6] = {
		{ptgTop, GreenB,  B1, pgtMiddle},
		{pgtMiddle, GreenB,  B0, pgtBottom},
		{pgtBottom, GreenB,  B2, ngtTop},						//FOR FUTURE CONNER DO THIS THING RN
		{ngtTop, YellowB, B0, ngtMiddle},						//WORKING IN THIS LAST
		{ngtMiddle, GreenB,  B3, ngtBottom},
		{ngtBottom, YellowB, B1, ptgTop},
	};

struct completeFSM {volatile uint8_t q; const uint8_t numTransitions; const struct qTransition *table;};
													//state					   //8

//pointer to array pushButton[0]
uint8_t getFSMoutput(struct completeFSM *x, uint8_t in){
	uint8_t out = 0;
	for(int row = 0; row < x->numTransitions; row++){
		//uint8_t old = (x->table[row].old);
		if (x->table[row].old == (x->q)){
			if(x->table[row].in == in){
				x->q = x->table[row].new;
				out = x->table[row].out;
				break;
			}
		}
	}
	return out;
}

//buttons set up
struct completeFSM btnInc = {.q = (uint8_t) 1, .numTransitions = 8, .table=pushButton};
struct completeFSM btnDec = {.q = (uint8_t) 1, .numTransitions = 8, .table=pushButton};
//Light FSM Setup
struct completeFSM lightFSM = {.q = (uint8_t) B1, .numTransitions = 8, .table=lightIncDec};


//While Loop Stuff
//Button INC input mask
volatile uint8_t inPA8 = (GPIOA-> IDR &(1<<8)) == 0;
if (inPA8 >= 1){
	inPA8 = 1;
}

//Button DEC input mask
volatile uint8_t inPB1 = (GPIOB-> IDR &(1<<1)) == 0;
if (inPB1 >= 1){
	inPB1 = 1;
}

//Update states
incCMD = getFSMoutput(&btnInc, inPA8);
for(volatile int n = 0; n<123456; n++){}

decCMD = getFSMoutput(&btnDec, inPB1);
for(volatile int n = 0; n<123456; n++){}

//DONT NEED THIS JUST USE THE SAME CALLING OF FXNS
//Blink lights
if((inPA8 == 1)&&(inPB1 != 1)){
//add 1
	display = getFSMoutput(&lightFSM, GreenB);
}
else if((inPA8 != 1) && (inPB1 == 1)){
	//subtract 1
	display = getFSMoutput(&lightFSM, YellowB);
}
else{
for(volatile int n = 0; n<12345; n++){}
	//update nothing
}

//Waiter
for(volatile int n = 0; n<1234567; n++){}

