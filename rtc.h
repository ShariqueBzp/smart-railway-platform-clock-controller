/*****************************************************************************
 * @file      rtc.h
 * @brief     Real-Time Clock Header File for LPC2148 Smart Railway Platform
 * @details   Prototypes for RTC initialization, time/date getters and setters, 
 *            day-of-week indexing, and LCD display helper routines using 
 *            centralized definitions from all_peripheral_defines.h.
 *****************************************************************************/

#ifndef RTC_H
#define RTC_H

#include "types.h"
#include "all_peripheral_defines.h"

/* Days of the Week Macros */
#define SUN 0
#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6

/* External Globals for Time, Date, and Day */
extern s32 hour, min, sec, date, month, year, day;
extern s8 week[7][4];

/* Function Prototypes */
void RTC_Init(void);
void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second);
void DisplayRTCTime(u32 hour, u32 minute, u32 second);
void GetRTCDateInfo(s32 *date, s32 *month, s32 *year);
void DisplayRTCDate(u32 date, u32 month, u32 year);
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second);
void SetRTCDateInfo(u32 date, u32 month, u32 year);
void GetRTCDay(s32 *dow);
void DisplayRTCDay(u32 day);
void SetRTCDay(u32 dow);

#endif /* RTC_H */
