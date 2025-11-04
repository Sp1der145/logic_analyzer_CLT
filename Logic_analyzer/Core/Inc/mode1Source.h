#ifndef MODE1SOURCE_H
#define MODE1SOURCE_H

#include "main.h"

/* ---------------------- ENUMS --------------------- */
// Enums for Encoder
enum encoderStates {UP, UP1q, UP2q, UP3q, DOWN, DW1q, DW2q, DW3q};
enum encoderOutput {PRESSED, UNPRESSED};
enum encoderInput {GPLOW, GPHIGH};
// Enums for button
enum buttonStates {PBqUP, PBqDOWN, PBqUPweak, PBqDOWNweak};
enum buttonOutput {PBUP, PBDOWN, PBRELEASE, PBPRESS};
enum buttonInput {PBlow, PBhigh};
enum bNum {ptgTop, pgtMiddle, pgtBottom, ngtTop, ngtMiddle, ngtBottom};
enum sampleRateLevel {SR_LOW, SR_MEDIUM, SR_HIGH};

/* ---------------------- STRUCTS --------------------- */
struct qTransition {
    uint8_t old;
    uint8_t in;
    uint8_t new;
    uint16_t out;
};

struct completeFSM {
    volatile uint8_t q;
    const uint8_t numTransitions;
    const struct qTransition *table;
};

/* ---------------------- EXTERN VARIABLES --------------------- */

// Input vars
extern volatile uint8_t P_A_in;
extern volatile uint8_t P_B_in;
extern volatile uint8_t C_A_in;
extern volatile uint8_t C_B_in;

// FSM structs
extern struct completeFSM EncA;
extern struct completeFSM EncB;
extern struct completeFSM btnInc;
extern struct completeFSM triggerTraceFSM;
extern struct completeFSM sampleRateFSM;

// Transition tables
extern const struct qTransition encoderEdgeTransitions[16];
extern const struct qTransition pushButton[8];
extern const struct qTransition triggerCycles[6];
extern const struct qTransition sampleRateTransitions[5];

// Shared queue data (with internal storage in mode1Source.c)
extern volatile uint8_t _triggerTrace;
extern volatile uint32_t _sampleRate;

/* ---------------------- FUNCTION PROTOTYPES --------------------- */
void setTriggerTrace(uint8_t val);
uint8_t getTriggerTrace(void);

void setSampleRate(uint8_t val);
uint32_t getSampleRate(void);

uint8_t getFSMoutput(struct completeFSM *x, uint8_t in);

void checkChannelA(void);
void checkChannelB(void);
void nextTriggerTrace(void);
void checkEncoderInputs(void);
void checkButtonFSM(void);

#endif // MODE1SOURCE_H
