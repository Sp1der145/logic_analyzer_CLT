#include "main.h"
#include "mode1Source.h"

/* ------------------------------------------------------ */
/* ------------------- Local Vars -------------------- */
/* ------------------------------------------------------ */

// Input vars
static uint8_t P_A_in = UNPRESSED;
static uint8_t P_B_in = UNPRESSED;
static uint8_t C_A_in;
static uint8_t C_B_in;

// GPIO input snapshots
static uint8_t inPA9;
static uint8_t inPA10;
static uint8_t inPA8;

// FSM structs
static struct completeFSM EncA = { .q = 1, .numTransitions = 16, .table = encoderEdgeTransitions };
static struct completeFSM EncB = { .q = 1, .numTransitions = 16, .table = encoderEdgeTransitions };
static struct completeFSM btnInc = { .q = PBqUP, .numTransitions = 8, .table = pushButton };
static struct completeFSM btnDec = { .q = PBqUP, .numTransitions = 8, .table = pushButton };
static struct completeFSM triggerTraceFSM = { .q = pgtMiddle, .numTransitions = 6, .table = triggerCycles };

/* ------------------------------------------------------ */
/* ------------------- PUBLIC QUEUE VARS ----------------- */
/* ------------------------------------------------------ */
volatile uint8_t _triggerTrace = 0;   // Shared queue variable
volatile uint32_t _sampleRate = 5;    // Shared queue variable

/* ------------------------------------------------------ */
/* ---------------------- FSM TABLES --------------------- */
/* ------------------------------------------------------ */

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

const struct qTransition triggerCycles[6] = {
    {ptgTop, 1, pgtMiddle, pgtMiddle},
    {pgtMiddle, 1, pgtBottom, pgtBottom},
    {pgtBottom, 1, ngtTop, ngtTop},
    {ngtTop, 1, ngtMiddle, ngtMiddle},
    {ngtMiddle, 1, ngtBottom, ngtBottom},
    {ngtBottom, 1, ptgTop, ptgTop},
};

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

    if (C_A_in != P_A_in) {
        if (C_A_in != C_B_in) {
            if (_sampleRate > 0) _sampleRate--;
        } else {
            if (_sampleRate < 10) _sampleRate++;
        }
        P_A_in = C_A_in;
    }
}

void checkChannelB(void) {
    updateInputs();

    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);

    if (C_B_in != P_B_in) {
        if (C_B_in == C_A_in) {
            if (_sampleRate > 0) _sampleRate--;
        } else {
            if (_sampleRate < 10) _sampleRate++;
        }
        P_B_in = C_B_in;
    }
}

void nextTriggerTrace(void) {
    updateInputs();
    uint8_t buttonPress = getFSMoutput(&btnInc, inPA8);
    uint8_t newTrace = getFSMoutput(&triggerTraceFSM, buttonPress);
    if (buttonPress == PBRELEASE) {
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
