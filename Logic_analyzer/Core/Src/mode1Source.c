#include "mode1Source.h"
#include "queue.h"
#include "main.h"
#include "mode1TimerISR.h"

/* ------------------------------------------------------ */
/* ---------------------- QUEUE VARS -------------------- */
/* ------------------------------------------------------ */
// Only queue vars are global (shared)
volatile uint8_t _triggerTrace = 0;  // Can be 0–5
volatile uint32_t _sampleRate = 5;   // Can be 0–10

//Queue for triggerTrace&sampleRate Task Sharing
extern Queue M1_trigger_q;
extern Queue M2_q;

/* ------------------------------------------------------ */
/* ------------------ LOCAL VARS --------------- */
/* ------------------------------------------------------ */
// GPIO input snapshots
static uint8_t inPA9;
static uint8_t inPA10;
static uint8_t inPA8;

// Previous pin states for edge detection
static uint8_t prevPB0 = 0;
static uint8_t prevPA12 = 0;
static uint8_t prevPB3 = 0;

/* ------------------------------------------------------ */
/* ---------------------- FSM TABLES --------------------- */
/* ------------------------------------------------------ */

// Encoder Edge Detector
static const struct qTransition encoderEdgeTransitions[16] = {
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
static const struct qTransition pushButton[8] = {
    {PBqUP, PBlow, PBqUPweak, PBUP},
    {PBqUP, PBhigh, PBqUP, PBUP},
    {PBqDOWN, PBlow, PBqDOWN, PBDOWN},
    {PBqDOWN, PBhigh, PBqDOWNweak, PBDOWN},
    {PBqUPweak, PBlow, PBqDOWN, PBPRESS},
    {PBqUPweak, PBhigh, PBqUP, PBUP},
    {PBqDOWNweak, PBlow, PBqDOWN, PBDOWN},
    {PBqDOWNweak, PBhigh, PBqUP, PBRELEASE},
};

// Trigger Cycle FSM (Outputs 0–5)
static const struct qTransition triggerCycles[6] = {
    {ptgTop, 1, pgtMiddle, pgtMiddle},
    {pgtMiddle, 1, pgtBottom, pgtBottom},
    {pgtBottom, 1, ngtTop, ngtTop},
    {ngtTop, 1, ngtMiddle, ngtMiddle},
    {ngtMiddle, 1, ngtBottom, ngtBottom},
    {ngtBottom, 1, ptgTop, ptgTop},
};

/* ------------------------------------------------------ */
/* ---------------------- FSM STRUCTS -------------------- */
/* ------------------------------------------------------ */
static struct completeFSM EncA = { .q = 1, .numTransitions = 16, .table = encoderEdgeTransitions };
static struct completeFSM EncB = { .q = 1, .numTransitions = 16, .table = encoderEdgeTransitions };
static struct completeFSM btnPush = { .q = PBqUP, .numTransitions = 8, .table = pushButton };
static struct completeFSM triggerTraceFSM = { .q = pgtMiddle, .numTransitions = 6, .table = triggerCycles };

/* ------------------------------------------------------ */
/* ---------------------- GETTERS/SETTERS ---------------- */
/* ------------------------------------------------------ */
void setTriggerTrace(uint8_t val) { _triggerTrace = val; }
uint8_t getTriggerTrace(void) { return _triggerTrace; }

void setSampleRate(uint8_t val) { _sampleRate = val; }
uint32_t getSampleRate(void) { return _sampleRate; }

/* ------------------------------------------------------ */
/* ---------------------- FSM HELPER FXN ----------------- */
/* ------------------------------------------------------ */
static uint8_t getFSMoutput(struct completeFSM *x, uint8_t in) {
    uint8_t out = 0;
    for (int row = 0; row < x->numTransitions; row++) {
        if (x->table[row].old == x->q && x->table[row].in == in) {
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
void checkChannelA_B(void) {
    updateInputs();

    // Debunce and gather Channel A & B
    uint8_t A_out = getFSMoutput(&EncA, inPA9);
    uint8_t B_out = getFSMoutput(&EncB, inPA10);

    // Remember the old A value (Set as 0/Unpressed first)
    static uint8_t oldA = UNPRESSED;

    // Only act when A changes from UNPRESSED -> PRESSED
	if (oldA == UNPRESSED && A_out == PRESSED) {
		// If Channel B is high, rotation is one direction; if low, the other.
		if (B_out) {
			//Inc Sample rate to 10 MAX
			if (_sampleRate < 10) _sampleRate++;
		} else {
			// Dec Sample rate to 1 MIN
			if (_sampleRate > 1) _sampleRate--;
		}
		// Update the Current SampleTimer
		updateSampleTimer_LL(_sampleRate);
	}
    oldA = A_out;

    // DOUBLE CHECK that sample rate is a between 1 & 10
    if (_sampleRate < 1) _sampleRate = 1;
    if (_sampleRate > 10) _sampleRate = 10;
}


void nextTriggerTrace(void) {
    updateInputs();
    uint8_t buttonOutput = getFSMoutput(&btnPush, inPA8);
    // Advance the trigger trace when the button is RELEASED
    // Which means that it had to be PRESSED in the first place hence why it only updates after release
    if (buttonOutput == PBRELEASE) {
        uint8_t newTrace = getFSMoutput(&triggerTraceFSM, 1);
        setTriggerTrace(newTrace);
    }
}


/* ------------------------------------------------------ */
/* ---------------------- TASK CALLERS ------------------ */
/* ------------------------------------------------------ */
void checkEncoderInputs(void) {
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5) == 0){
		checkChannelA_B();
	}
	enqueue(&M2_q, 0x02000000 | _sampleRate);
}

void checkButtonFSM(void) {
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5) == 0){
		nextTriggerTrace();
	}
	enqueue(&M2_q, 0x01000000 | _triggerTrace);
}

void checkTriggerEdges(void) {
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5) == 0){
		uint8_t curPB0 = LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0);
		uint8_t curPA12 = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12);
		uint8_t curPB3 = LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3);

		// Positive edges
		if (!prevPB0 && curPB0) enqueue(&M1_trigger_q, ptgTop);
		if (!prevPA12 && curPA12) enqueue(&M1_trigger_q, pgtMiddle);
		if (!prevPB3 && curPB3) enqueue(&M1_trigger_q, pgtBottom);

		// Negative edges
		if (prevPB0 && !curPB0) enqueue(&M1_trigger_q, ngtTop);
		if (prevPA12 && !curPA12) enqueue(&M1_trigger_q, ngtMiddle);
		if (prevPB3 && !curPB3) enqueue(&M1_trigger_q, ngtBottom);

		prevPB0 = curPB0;
		prevPA12 = curPA12;
		prevPB3 = curPB3;
	}
}
