/*
 * mode1TimerISR.c
 *
 *  Created on: Nov 10, 2025
 *      Author: conne
 */

#include "mode1TimerISR.h"
// NEED TO CHANGE ONCE THE QUEUES HAVE BEEN SET UP
// For access to getSampleRate()
#include "mode1Source.h"
#include "main.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"


/* ------------------------------------------------------ */
/* ---------------------- Globals ----------------------- */
/* ------------------------------------------------------ */
// Initial sample period
volatile uint32_t samplePeriod = 1000;

/* ------------------------------------------------------ */
/* ------------ Timer Initialization (LL) --------------- */
/* ------------------------------------------------------ */
void initSampleTimer_LL(void)
{
    // Enable TIM6 clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

    // Configure TIM6
    // Adjust as needed
    LL_TIM_SetPrescaler(TIM6, 16 - 1);
    LL_TIM_SetAutoReload(TIM6, samplePeriod-1);
    LL_TIM_SetCounterMode(TIM6, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableARRPreload(TIM6);

    // Enable update interrupt
    LL_TIM_EnableIT_UPDATE(TIM6);

    // Enable IRQ (in NVIC)
    NVIC_SetPriority(TIM6_DAC_IRQn, 0);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);

    // Start the counter
    LL_TIM_EnableCounter(TIM6);
}

/* ------------------------------------------------------ */
/* ---------------- Update Timer Period ----------------- */
/* ------------------------------------------------------ */
void updateSampleTimer_LL(uint32_t newPeriod)
{
    samplePeriod = newPeriod;
    // Temporarily stop timer
    LL_TIM_DisableCounter(TIM6);
    // Update auto-reload register
    LL_TIM_SetAutoReload(TIM6, samplePeriod - 1);
    // Reset counter
    LL_TIM_SetCounter(TIM6, 0);
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
        // Call sampling routine
        // EXAMPLE: collectSample();
        //Need to talk with Logan to see how we want to set this up
    }
}
