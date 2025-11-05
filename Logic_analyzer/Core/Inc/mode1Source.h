#ifndef MODE1SOURCE_H_
#define MODE1SOURCE_H_

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

// Enum for trigger trace FSM
enum bNum {ptgTop, pgtMiddle, pgtBottom, ngtTop, ngtMiddle, ngtBottom};

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
/*
 * Only queue-related globals are shared — all other vars and FSMs
 * are static and private to mode1Source.c.
 */
extern volatile uint8_t _triggerTrace;
extern volatile uint32_t _sampleRate;

/* ---------------------- FUNCTION PROTOTYPES --------------------- */
void setTriggerTrace(uint8_t val);
uint8_t getTriggerTrace(void);

void setSampleRate(uint8_t val);
uint32_t getSampleRate(void);

void checkChannelA(void);
void checkChannelB(void);
void nextTriggerTrace(void);
void checkEncoderInputs(void);
void checkButtonFSM(void);

#endif /* MODE1SOURCE_H_ */
