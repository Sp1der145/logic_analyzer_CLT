/*
 * dataCollection.h
 *
 *  Created on: Nov 11, 2025
 *      Author: conne
 */

#ifndef INC_DATACOLLECTION_H_
#define INC_DATACOLLECTION_H_

#include "stm32l4xx_ll_gpio.h"
#include "queue.h"
#include <stdint.h>

extern Queue M1_setup_q;
extern Queue M1_trigger_q;
extern Queue M1_data_q;

void dataCollectionTask(void);

#endif /* INC_DATACOLLECTION_H_ */
