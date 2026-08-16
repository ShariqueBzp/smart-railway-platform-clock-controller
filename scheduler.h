/*****************************************************************************
 * @file      scheduler.h
 * @brief     RTC and Train Database Comparison Scheduler Header File
 * @details   Prototypes for background monitoring tasks that match real-time clock 
 *            (RTC) values against train arrival/departure schedules and handle 
 *            live display updates on the 2x16 LCD.
 *****************************************************************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include "all_peripheral_defines.h"

/* Function Prototypes */
void Init_Scheduler(void);
void CheckTrainSchedules(void);
void DisplayActiveTrainStatus(void);

#endif /* SCHEDULER_H */

