/*
 * mode1TimerISR.h
 *
 *  Created on: Nov 10, 2025
 *      Author: conne
 */

#ifndef INC_MODE1TIMERISR_H_
#define INC_MODE1TIMERISR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
extern volatile uint32_t samplePeriod;
extern volatile uint8_t sampleReadyFlag;

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief  Initialize TIM6 as periodic timer with interrupt (LL-based)
 *         Starts the timer immediately.
 */
void initSampleTimer(void);

/**
 * @brief  Update the sample rate (timer period)
 * @param  newPeriod  New period value (ARR + 1)
 */
void updateSampleTimer_LL(uint32_t newPeriod);

/**
 * @brief  Placeholder for user sampling logic.
 *         Implemented in mode1TimerISR.c.
 */
void collectSample(void);

/**
 * @brief  TIM6 IRQ handler (declared here so main.c can reference it if needed)
 */
void TIM7_DAC_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_MODE1TIMERISR_H_ */

