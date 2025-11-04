






/* ---------------------- ENUMS --------------------- */
// Enums for Encoder
enum encoderStates {UP, UP1q, UP2q, UP3q, DOWN, DW1q, DW2q, DW3q};
enum encoderOutput {PRESSED, UNPRESSED};
// Enums for button
enum buttonStates{PBqUP, PBqDOWN, PBqUPweak, PBqDOWNweak};
enum buttonOutput{PBUP, PBDOWN, PBRELEASE, PBPRESS};
enum buttonInput{PBlow, PBhigh};
enum bNum{ptgTop, pgtMiddle, pgtBottom, ngtTop, ngtMiddle, ngtBottom};

/* ---------------------- INITS --------------------- */
//Input Var Initialization
volatile uint8_t P_A_in = UNPRESSED;
volatile uint8_t P_B_in = UNPRESSED;

//GPIO IN A
volatile uint8_t C_A_in;
//GPIO IN B
volatile uint8_t C_B_in;
// Encoder Channel A Input Mask			WILL NEED A DIFF INPUT PIN		INPUT MASK FOR GPIO (CHANGE INPUT PIN)
volatile uint8_t inPA9 = (GPIOA->IDR & (1 << 9)) == 0;
// Encoder Channel B Input Mask 		WILL NEED A DIFF INPUT PIN		INPUT MASK FOR GPIO (CHANGE INPUT PIN)
volatile uint8_t inPA10 = (GPIOA->IDR & (1 << 10)) == 0;

//Button input mask						WILL NEED A DIFF INPUT PIN		INPUT MASK FOR GPIO (CHANGE INPUT PIN)
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

// Trigger Cycle FSM
const struct qTransition triggerCycles[6] = {
		{ptgTop, 	buttonPress,  pgtMiddle, pgtMiddle},//Out 1 	USE A GETTER to have the output gotten
		{pgtMiddle, buttonPress,  pgtBottom, pgtBottom},//Out 2
		{pgtBottom, buttonPress,  ngtTop, 	 ngtTop   },//Out 3
		{ngtTop, 	buttonPress,  ngtMiddle, ngtMiddle},//Out 4	  	SO FUTURE CONNER TALK TO LOGAN
		{ngtMiddle, buttonPress,  ngtBottom, ngtBottom},//Out 5   	CONNECT THESE 2 FUNCTIONALITIES
		{ngtBottom, buttonPress,  ptgTop, 	 ptgTop   },//Out 0
	};

// Sample rate FSM
const struct qTransition sampleRateTransitions[6] = {
    {SR_LOW, PRESSED, SR_MEDIUM, 5000},   // increase
    {SR_MEDIUM, PRESSED, SR_HIGH, 10000}, // increase
    {SR_HIGH, PRESSED, SR_LOW, 1000},     // wrap
    {SR_HIGH, UNPRESSED, SR_MEDIUM, 5000},// decrease
    {SR_MEDIUM, UNPRESSED, SR_LOW, 1000}, // decrease
};


/* ---------------------- CW/CCW Checker START --------------------- */
// FSM Structs for Encoder pin A & Encoder pin B
struct completeFSM EncA = { .q = (uint8_t) 1, .numTransitions = 16, .table = encoderEdgeTransitions };
struct completeFSM EncB = { .q = (uint8_t) 1, .numTransitions = 16, .table = encoderEdgeTransitions };

/* ---------------------------------------------- */
/* ---------------------- FSM's ----------------- */
/* ---------------------------------------------- */
struct completeFSM EncA = {.q = UP, .numTransitions = 16, .table = encoderEdgeTransitions};
struct completeFSM EncB = {.q = UP, .numTransitions = 16, .table = encoderEdgeTransitions};
struct completeFSM btnInc = {.q = PBqUP, .numTransitions = 8, .table = pushButton};
struct completeFSM triggerTraceFSM = {.q = pgtMiddle, .numTransitions = 6, .table = triggerCycles};
struct completeFSM sampleRateFSM = {.q = SR_MEDIUM, .numTransitions = 5, .table = sampleRateTransitions};




