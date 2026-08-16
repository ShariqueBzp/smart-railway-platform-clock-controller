/*****************************************************************************
 * @file     trainDB.c
 * @brief    Train Database Source File for LPC2148 Smart Railway Platform
 * @details  Stores schedule details for trains.
 *****************************************************************************/

#include "trainDB.h"

TrainInfo_t TrainDB[TOTAL_TRAINS] = {
    {
        12627,                              /* trainNumber */
        "Karnataka Exp",                    /* trainName */
        "Bangalore",                        /* destination */
        12, 0,                              /* arrivalHour, arrivalMinute */
        12, 10,                             /* departureHour, departureMinute */
        12, 0,                              /* updatedArrivalHour, updatedArrivalMinute */
        12, 10,                             /* updatedDepartureHour, updatedDepartureMinute */
        1,                                  /* platform */
        0,                                  /* delayMinutes */
        0                                   /* trainStatus (STATUS_ON_TIME) */
    },
    {
        12028,                              /* trainNumber */
        "Shatabdi Exp",                     /* trainName */
        "Chennai",                          /* destination */
        15, 30,                             /* arrivalHour, arrivalMinute */
        15, 40,                             /* departureHour, departureMinute */
        15, 30,                             /* updatedArrivalHour, updatedArrivalMinute */
        15, 40,                             /* updatedDepartureHour, updatedDepartureMinute */
        2,                                  /* platform */
        0,                                  /* delayMinutes */
        0                                   /* trainStatus (STATUS_ON_TIME) */
    },
    {
        17652,                              /* trainNumber */
        "Kacheguda Exp",                    /* trainName */
        "Hyderabad",                        /* destination */
        18, 15,                             /* arrivalHour, arrivalMinute */
        18, 25,                             /* departureHour, departureMinute */
        18, 15,                             /* updatedArrivalHour, updatedArrivalMinute */
        18, 25,                             /* updatedDepartureHour, updatedDepartureMinute */
        3,                                  /* platform */
        0,                                  /* delayMinutes */
        0                                   /* trainStatus (STATUS_ON_TIME) */
    },
    {
        12791,                              /* trainNumber */
        "PNBE Express",                     /* trainName */
        "Patna",                            /* destination */
        9, 45,                              /* arrivalHour, arrivalMinute */
        10, 00,                              /* departureHour, departureMinute */
        9, 45,                              /* updatedArrivalHour, updatedArrivalMinute */
        10, 0,                              /* updatedDepartureHour, updatedDepartureMinute */
        4,                                  /* platform */
        0,                                  /* delayMinutes */
        0                                   /* trainStatus (STATUS_ON_TIME) */
    },
    {
        12393,                              /* trainNumber */
        "Sampoorna Exp",                    /* trainName */
        "New Delhi",                        /* destination */
        17, 10,                             /* arrivalHour, arrivalMinute */
        17, 25,                             /* departureHour, departureMinute */
        17, 10,                             /* updatedArrivalHour, updatedArrivalMinute */
        17, 25,                             /* updatedDepartureHour, updatedDepartureMinute */
        5,                                  /* platform */
        0,                                  /* delayMinutes */
        0                                   /* trainStatus (STATUS_ON_TIME) */
    },
    {
        12702,                              /* trainNumber */
        "Hussain Sagar",                    /* trainName */
        "Pune",                             /* destination */
        21, 30,                             /* arrivalHour, arrivalMinute */
        21, 45,                             /* departureHour, departureMinute */
        21, 30,                             /* updatedArrivalHour, updatedArrivalMinute */
        21, 45,                             /* updatedDepartureHour, updatedDepartureMinute */
        6,                                  /* platform */
        0,                                  /* delayMinutes */
        0                                   /* trainStatus (STATUS_ON_TIME) */
    }
};