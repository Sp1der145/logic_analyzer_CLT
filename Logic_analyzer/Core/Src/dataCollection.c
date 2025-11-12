/*
 * dataCollection.c
 *
 *  Created on: Nov 11, 2025
 *      Author: conne
 */

#include "dataCollection.h"
#include "queue.h"
#include "main.h"
#include "mode1Source.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_bus.h"

#define SAMPLE_SIZE 8

/* ------------------------------------------------------ */
/* ---------------------- LOCAL VARS -------------------- */
/* ------------------------------------------------------ */
static uint8_t trace1Array[SAMPLE_SIZE];
static uint8_t trace2Array[SAMPLE_SIZE];
static uint8_t trace3Array[SAMPLE_SIZE];

static uint8_t isCollecting = 0;
static uint8_t sampleIndex = 0;
static int32_t triggerType = -1;

void configurePinsForMode1(void) {
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5)){
		//Disable SPI peripheral if it's active
		LL_SPI_Disable(SPI1);

		//Set PB0, PA12, PB3 as INPUT
		LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
		LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_INPUT);
		LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_INPUT);
	}
}

void dataCollectionTask(void) {
    int32_t sampleEvent;
    int32_t triggerEvent;
    if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_5)){
		// Check for new trigger events
		if (dequeue(&M1_trigger_q, &triggerEvent)) {
			triggerType = triggerEvent;
			isCollecting = 0;
			sampleIndex = 0;
		}

		// Check for sample ticks from the timer
		if (dequeue(&M1_setup_q, &sampleEvent)) {
			// If we have a trigger, check which edge condition met
			if (!isCollecting && triggerType >= 0) {
				switch (triggerType) {
					case ptgTop:     // positive edge PB0 - Trace 1
						if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0))
							isCollecting = 1;
						break;
					case pgtMiddle:  // positive edge PA12 - Trace 2
						if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12))
							isCollecting = 1;
						break;
					case pgtBottom:  // positive edge PB3 - Trace 3
						if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3))
							isCollecting = 1;
						break;
					case ngtTop:     // negative edge PB0 - Trace 1
						if (!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0))
							isCollecting = 1;
						break;
					case ngtMiddle:  // negative edge PA12 - Trace 2
						if (!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12))
							isCollecting = 1;
						break;
					case ngtBottom:  // negative edge PB3 - Trace 3
						if (!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3))
							isCollecting = 1;
						break;
				}
			}

			// Collect data samples
			if (isCollecting) {
				trace1Array[sampleIndex] = LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0);
				trace2Array[sampleIndex] = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12);
				trace3Array[sampleIndex] = LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3);

				sampleIndex++;

				if (sampleIndex >= SAMPLE_SIZE) {
					// Sampling complete — enqueue array pointers
					enqueue(&M1_data_q, (int32_t)trace1Array);
					enqueue(&M1_data_q, (int32_t)trace2Array);
					enqueue(&M1_data_q, (int32_t)trace3Array);

					isCollecting = 0; //Reset Collecting Val
					triggerType = -1; //Set to impossible trigger type val
				}
			}
		}
    }
}
