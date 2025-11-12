/*
 * mode1TimerISR.c
 *
 *  Created on: Nov 10, 2025
 *      Author: conne
 */

#include "mode1TimerISR.h"
#include "queue.h"
#include "main.h"
#include "dataCollection.h"

/* ------------------------------------------------------ */
/* ---------------------- Globals ----------------------- */
/* ------------------------------------------------------ */
volatile uint32_t samplePeriod = 1000;   // Initial sample period (ARR)
volatile uint8_t sampleReadyFlag = 0;    // Optional: flag for main loop

/* ------------------------------------------------------ */
/* ---------------------- Externs ----------------------- */
/* ------------------------------------------------------ */
extern Queue M1_setup_q;  // Mode 1 queue that contains the sample rate info

/* ------------------------------------------------------ */
/* ------------ Timer Initialization (LL) --------------- */
/* ------------------------------------------------------ */
void initSampleTimer(void) {
    // Configure LL timer for periodic interrupts
    LL_TIM_SetPrescaler(TIM6, 79);      // Adjust for desired freq
    LL_TIM_SetAutoReload(TIM6, 999);    // Period count
    LL_TIM_EnableIT_UPDATE(TIM6);
    LL_TIM_EnableCounter(TIM6);
}

/* ------------------------------------------------------ */
/* ---------------- Update Timer Period ----------------- */
/* ------------------------------------------------------ */
void updateSampleTimer_LL(uint32_t newPeriod)
{
	samplePeriod = newPeriod;
    LL_TIM_DisableCounter(TIM6);
    // Update auto-reload register
    LL_TIM_SetAutoReload(TIM6, samplePeriod - 1);
    // Restart timer
    LL_TIM_EnableCounter(TIM6);
}

/* ------------------------------------------------------ */
/* --------------- TIM6 IRQ Handler --------------------- */
/* ------------------------------------------------------ */
void TIM6_DAC_IRQHandler(void)
{
    // Check update flag
    if(LL_TIM_IsActiveFlag_UPDATE(TIM6))
    {
        // Clear interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM6);
        // Enqueue a sample event (simple counter or flag)
        enqueue(&M1_setup_q, 1);
    }
}
