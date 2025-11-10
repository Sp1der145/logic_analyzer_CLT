/*
 * mode1TimerISR.c
 *
 *  Created on: Nov 10, 2025
 *      Author: conne
 */

#include "mode1TimerISR.h"
#include "mode1Source.h"   // For access to getSampleRate()
#include "main.h"

/* ------------------------------------------------------ */
/* ---------------------- EXTERNAL TIMER ----------------- */
/* ------------------------------------------------------ */
// TBH don't know if this is the correct timer handler needs to match our CubeMX Setup so..
// Hopefully we can figure out which one is correct
extern TIM_HandleTypeDef htim2;

/* ------------------------------------------------------ */
/* ---------------------- CONFIGURATION ------------------ */
/* ------------------------------------------------------ */
// Base timer frequency (US MicroSeconds)
#define BASE_PERIOD_US 100000  // 100 ms base period

/* ------------------------------------------------------ */
/* ---------------------- INITIALIZATION ----------------- */
/* ------------------------------------------------------ */
void initSampleTimer(void) {
    // Ensure timer starts in interrupt mode
    HAL_TIM_Base_Start_IT(&htim2);
}

/* ------------------------------------------------------ */
/* ---------------------- PERIOD UPDATE ------------------ */
/* ------------------------------------------------------ */
void updateSampleTimer(void) {
    // Stop timer before reconfiguring
    HAL_TIM_Base_Stop_IT(&htim2);

    // Set new period based on sample rate
    uint32_t newPeriod = BASE_PERIOD_US / _sampleRate;

    // Convert microseconds to timer ticks
    // Assuming timer clock = 1 MHz (1 tick = 1 µs)
    __HAL_TIM_SET_AUTORELOAD(&htim2, newPeriod - 1);

    // Restart timer
    HAL_TIM_Base_Start_IT(&htim2);
}

/* ------------------------------------------------------ */
/* ---------------------- TIMER ISR ---------------------- */
/* ------------------------------------------------------ */
// Call automatically by HAL when timer period elapses
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        performDataSample();
    }
}
