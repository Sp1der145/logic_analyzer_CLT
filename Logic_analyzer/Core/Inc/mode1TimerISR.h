/*
 * mode1TimerISR.h
 *
 *  Created on: Nov 10, 2025
 *      Author: conne
 */

#ifndef INC_MODE1TIMERISR_H_
#define INC_MODE1TIMERISR_H_


#include "main.h"

/* ---------------------- FUNCTION PROTOTYPES --------------------- */
void initSampleTimer(void);
void updateSampleTimer(void);
void performDataSample(void);


#endif /* INC_MODE1TIMERISR_H_ */
