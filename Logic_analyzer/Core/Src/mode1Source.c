/*
 * mode1Source.c
 *
 *  Created on: Oct 8, 2025
 *      Author: crupton23
 */

#include "main.h"
#include "mode1Source.h"

/* ------------------------------------------------------ */
/* ---------------------- QUEUE STUFF -------------------- */
/* ------------------------------------------------------ */
void setTriggerTrace(uint8_t val) {
    _triggerTrace = val;
}
uint8_t getTriggerTrace(void) {
    return _triggerTrace;
}

void setSampleRate(uint32_t val) {
    _sampleRate = val;
}
uint32_t getSampleRate(void) {
    return _sampleRate;
}


/* ------------------------------------------------------ */
/* ---------------------- INITS -------------------------- */
/* ------------------------------------------------------ */
// Encoder Channel A Input Mask – WILL NEED A DIFF INPUT PIN
volatile uint8_t inPA9;
// Encoder Channel B Input Mask – WILL NEED A DIFF INPUT PIN
volatile uint8_t inPA10;
// Button input mask – WILL NEED A DIFF INPUT PIN
volatile uint8_t inPA8;


/* ------------------------------------------------------ */
/* ---------------------- FSM OUTPUT FXN ----------------- */
/* ------------------------------------------------------ */
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
/* ---------------------- INPUT READ FXN ----------------- */
/* ------------------------------------------------------ */
static inline void updateInputs(void) {
    inPA9  = (GPIOA->IDR & (1 << 9))  ? 1 : 0;
    inPA10 = (GPIOA->IDR & (1 << 10)) ? 1 : 0;
    inPA8  = (GPIOA->IDR & (1 << 8))  ? 1 : 0;
}

/* ------------------------------------------------------ */
/* ---------------------- FXN'S -------------------------- */
/* ------------------------------------------------------ */
// Get the input from Channel A pin for the Encoder
void checkChannelA(void) {
    updateInputs();

    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);

    if (C_A_in != P_A_in) {
        uint32_t newRate = getFSMoutput(&sampleRateFSM,
                            (C_A_in != C_B_in) ? PRESSED : UNPRESSED);
        setSampleRate(newRate);
        P_A_in = C_A_in;
    }
}

// Get the input from Channel B pin for the Encoder
void checkChannelB(void) {
    updateInputs();

    C_A_in = getFSMoutput(&EncA, inPA9);
    C_B_in = getFSMoutput(&EncB, inPA10);

    if (C_B_in != P_B_in) {
        uint32_t newRate = getFSMoutput(&sampleRateFSM,
                            (C_B_in == C_A_in) ? PRESSED : UNPRESSED);
        setSampleRate(newRate);
        P_B_in = C_B_in;
    }
}

// Handle button FSM and trigger trace
void nextTriggerTrace(void) {
    updateInputs();

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
