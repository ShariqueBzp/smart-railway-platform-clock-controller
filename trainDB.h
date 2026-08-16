/*****************************************************************************
 * @file      trainDB.h
 * @brief     Train Database Header File for LPC2148 Smart Railway Platform
 * @details   Defines the TrainInfo_t structure using types.h for consistency, 
 *            readability, and portability across peripherals.
 *****************************************************************************/

#ifndef TRAIN_DB_H
#define TRAIN_DB_H

#include "types.h"

#define TOTAL_TRAINS 6

/* Train Status Definitions */
#define STATUS_ON_TIME    1
#define STATUS_ARRIVED    2
#define STATUS_DEPARTED   3

typedef struct {
    unsigned int trainNumber;
    char trainName[25];
    char destination[20];
    u8 arrivalHour;
    u8 arrivalMinute;
    u8 departureHour;
    u8 departureMinute;
    u8 updatedArrivalHour;
    u8 updatedArrivalMinute;
    u8 updatedDepartureHour;
    u8 updatedDepartureMinute;
    u8 platform;
    u8 delayMinutes;
    u8 trainStatus; /* Added to resolve scheduler.c errors */
} TrainInfo_t;

/* External declaration of the train database array */
extern TrainInfo_t TrainDB[TOTAL_TRAINS];

#endif /* TRAIN_DB_H */

