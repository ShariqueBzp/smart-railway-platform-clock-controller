/*****************************************************************************
 * @file      rtc.c
 * @brief     Real-Time Clock Driver Source File for LPC2148 Smart Railway Platform
 * @details   Implements hardware configuration for the built-in RTC on LPC2148, 
 *            clock/date management, and formatting routines utilizing centralized 
 *            macros and prescaler calculations entirely from all_peripheral_defines.h.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "lcd.h"
#include "all_peripheral_defines.h"
#include "rtc.h"

/* RTC Control Register (CCR) Bit Definitions */
#define RTC_ENABLE  (1 << 0)
#define RTC_RESET   (1 << 1)
#define RTC_CLKSRC  (1 << 4)   /* LPC2148 external 32.768 kHz oscillator source select */

#define CPU_LPC2148

/* Global Variables Definition */
s32 hour, min, sec, date, month, year, day;

/* Array to hold names of days of the week */
s8 week[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

/* Initialize the Real-Time Clock (RTC) using centralized prescaler definitions */
void RTC_Init(void) {
    /* Disable and reset the RTC */
    CCR = RTC_RESET;
    
#ifndef CPU_LPC2124
    /* Set prescaler integer and fractional parts using consolidated definitions */
    PREINT  = PREINT_VALUE;
    PREFRAC = PREFRAC_VALUE;
    
    /* Enable the RTC */
    CCR = RTC_ENABLE;  
#else
    /* Enable the RTC with external 32.768 kHz clock source */
    CCR = RTC_ENABLE | RTC_CLKSRC;  
#endif
}

/* Get the current RTC time */
void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second) {
    *hour   = HOUR;
    *minute = MIN;
    *second = SEC;
}

/* Display the RTC time on LCD */
void DisplayRTCTime(u32 hour, u32 minute, u32 second) {
    CmdLCD(GOTO_LINE1_POSO);
    CharLCD((hour / 10) + 48);
    CharLCD((hour % 10) + 48);
    CharLCD(':');
    CharLCD((minute / 10) + 48);
    CharLCD((minute % 10) + 48);
    CharLCD(':');
    CharLCD((second / 10) + 48);
    CharLCD((second % 10) + 48);
}

/* Get the current RTC date */
void GetRTCDateInfo(s32 *date, s32 *month, s32 *year) {
    *date  = DOM;
    *month = MONTH;
    *year  = YEAR;
}

/* Display the RTC date on LCD */
void DisplayRTCDate(u32 date, u32 month, u32 year) {
    CmdLCD(GOTO_LINE2_POSO);
    CharLCD((date / 10) + 48);
    CharLCD((date % 10) + 48);
    CharLCD('/');
    CharLCD((month / 10) + 48);
    CharLCD((month % 10) + 48);
    CharLCD('/');
    U32LCD(year);
}

/* Set the RTC time */
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second) {
    HOUR = hour;
    MIN  = minute;
    SEC  = second;
}

/* Set the RTC date */
void SetRTCDateInfo(u32 date, u32 month, u32 year) {
    DOM   = date;
    MONTH = month;
    YEAR  = year;
}

/* Get the current day of the week */
void GetRTCDay(s32 *dow) {
    *dow = DOW;  
}

/* Display the current day of the week on LCD */
void DisplayRTCDay(u32 day) {
    CmdLCD(GOTO_LINE1_POSO + 10);
    StrLCD(week[day]);  
}

/* Set the day of the week in RTC */
void SetRTCDay(u32 dow) {
    DOW = dow;
}
