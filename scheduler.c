/*****************************************************************************
 * @file    scheduler.c
 * @brief   Scheduler & Train Announcement Dual-Mode Logic Source File
 * @details Compares RTC time with TrainDB schedules, cycles through all trains 
 *          in Window 1, locks strictly to the specific upcoming train in Window 2,
 *          and manages active arrival/departure states in Window 3.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "lcd.h"
#include "kpm.h"
#include "all_peripheral_defines.h"  /* Contains GOTO_LINE1_POSO, GOTO_LINE2_POSO */
#include "delay.h"
#include "trainDB.h"
#include "rtc.h"
#include "scheduler.h"

extern void CmdLCD(u8 opcode);
extern void StrLCD(s8 *str);

/* Initialize scheduler parameters */
void Init_Scheduler(void)
{
    /* Initialization if needed */
}

/* Compare current RTC time with TrainDB schedules and assign status */
void CheckTrainSchedules(void)
{
    u8 i;
    u32 currentMins = (HOUR * 60) + MIN;

    for(i = 0; i < TOTAL_TRAINS; i++)
    {
        u32 arrMins = (TrainDB[i].updatedArrivalHour * 60) + TrainDB[i].updatedArrivalMinute;
        u32 depMins = (TrainDB[i].updatedDepartureHour * 60) + TrainDB[i].updatedDepartureMinute;

        if(currentMins < arrMins)
            TrainDB[i].trainStatus = STATUS_ON_TIME;
        else if(currentMins >= arrMins && currentMins <= depMins)
            TrainDB[i].trainStatus = STATUS_ARRIVED;
        else
            TrainDB[i].trainStatus = STATUS_DEPARTED;
    }
}

/* Helper function to get string length safely */
static u8 GetStringLength(s8 *str)
{
    u8 len = 0;
    while(str[len] != '\0' && len < 40)
    {
        len++;
    }
    return len;
}

/* Main Display Routine Handling Window 1 (All Trains Cycling), Window 2 (Upcoming Train), and Window 3 */
void DisplayActiveTrainStatus(void)
{
    static u16 scrollPos1 = 0;
    static u32 scrollTimer = 0;
    static u8 stateStep = 0;         
    static u32 stateTimer = 0;       
    static u8 trainDisplayIdx = 0;   
    static u32 trainSwitchTimer = 0;
    static u8 prevActiveWindow = 0;

    u32 currentMins;
    u32 currentTotalSecs;
    u32 minFutureArr = 0xFFFFFFFF; 
    u32 minOverallArr = 0xFFFFFFFF;
    s8 futureTrainIdx = -1;

    s8 earliestTrainIdx = 0;
    
    u8 i = 0;
    u8 j = 0;
    u8 k = 0;
    u8 len1 = 0;
    u8 delayMins = 0;
    
    s8 line1Text[17];
    s8 line2Text[17];
    s8 scrollBuffer1[64];
    TrainInfo_t *t;

    u32 arrMins;
    u32 depMins;
    u32 arrTotalSecs;
    u32 depTotalSecs;
    u8 activeWindow;
    
    s8 headerText[] = "PF  AT  DT  DLY ";
    s8 arrStatus[]  = "CAME";
    s8 depStatus[]  = "GONE";

    currentMins = (HOUR * 60) + MIN;
    currentTotalSecs = (HOUR * 3600) + (MIN * 60) + SEC;

    /* Scan database to find next upcoming train today or earliest overall train */
    for(i = 0; i < TOTAL_TRAINS; i++)
    {
        u32 tempArrMins = (TrainDB[i].updatedArrivalHour * 60) + TrainDB[i].updatedArrivalMinute;
        
        if(tempArrMins < minOverallArr)
        {
            minOverallArr = tempArrMins;
            earliestTrainIdx = i;
        }

        if(tempArrMins >= currentMins)
        {
            if(tempArrMins < minFutureArr)
            {
                minFutureArr = tempArrMins;
                futureTrainIdx = i;
            }
        }
    }

    /* First, provisionally check if any train is currently in Window 2 or Window 3 */
    activeWindow = 1; /* Default: Window 1 */
    
    for(i = 0; i < TOTAL_TRAINS; i++)
    {
        u32 tArrMins = TrainDB[i].updatedArrivalHour * 60 + TrainDB[i].updatedArrivalMinute;
        u32 tDepMins = TrainDB[i].updatedDepartureHour * 60 + TrainDB[i].updatedDepartureMinute;
        
        u32 tArrSecs = (TrainDB[i].updatedArrivalHour * 3600) + (TrainDB[i].updatedArrivalMinute * 60);
        u32 tDepSecs = (TrainDB[i].updatedDepartureHour * 3600) + (TrainDB[i].updatedDepartureMinute * 60);

        /* Window 2 check: within 5 minutes prior to arrival */
        if(currentMins >= (tArrMins >= 5 ? tArrMins - 5 : 0) && currentMins < tArrMins)
        {
            activeWindow = 2;
            trainDisplayIdx = i; /* STRICTLY LOCK to this specific upcoming train */
            break;
        }
        /* Window 3 check: from arrival time up to 30 seconds after departure time */
        else if(currentTotalSecs >= tArrSecs && currentTotalSecs <= (tDepSecs + 30))
        {
            activeWindow = 3;
            trainDisplayIdx = i; /* STRICTLY LOCK to this active train */
            break;
        }
    }

    /* Reset state machine step when transitioning between windows to prevent glitching */
    if(activeWindow != prevActiveWindow)
    {
        stateStep = 0;
        stateTimer = 0;
        prevActiveWindow = activeWindow;
    }

    /* If not in Window 2 or 3, we are in Window 1: Automatically cycle through all trains */
    if(activeWindow == 1)
    {
        trainSwitchTimer++;
        if(trainSwitchTimer >= 150)
        {
            trainSwitchTimer = 0;
            trainDisplayIdx = (trainDisplayIdx + 1) % TOTAL_TRAINS;
        }
    }

    t = &TrainDB[trainDisplayIdx];
    arrMins = (t->updatedArrivalHour * 60) + t->updatedArrivalMinute;
    depMins = (t->updatedDepartureHour * 60) + t->updatedDepartureMinute;
    arrTotalSecs = (t->updatedArrivalHour * 3600) + (t->updatedArrivalMinute * 60);
    depTotalSecs = (t->updatedDepartureHour * 3600) + (t->updatedDepartureMinute * 60);

    /* --- ABSOLUTE BUFFER RESET: Fill entire arrays with space characters right at the start --- */
    for(i = 0; i < 16; i++)
    {
        line1Text[i] = ' ';
        line2Text[i] = ' ';
    }
    line1Text[16] = '\0';
    line2Text[16] = '\0';

    /* Build Scroll Buffer for Line 1 (Train Name & Destination) */
    i = 0;
    while(t->trainName[i] != '\0' && i < 25)
    {
        scrollBuffer1[i] = t->trainName[i];
        i++;
    }
    
    scrollBuffer1[i++] = ' ';
    scrollBuffer1[i++] = '-';
    scrollBuffer1[i++] = '>';
    scrollBuffer1[i++] = ' ';
    
    k = 0;
    while(t->destination[k] != '\0' && i < 50)
    {
        scrollBuffer1[i++] = t->destination[k++];
    }
    
    scrollBuffer1[i++] = ' ';
    scrollBuffer1[i++] = ' ';
    scrollBuffer1[i++] = ' ';
    scrollBuffer1[i++] = ' ';
    scrollBuffer1[i] = '\0';

    len1 = GetStringLength(scrollBuffer1);

    /* --- Window Handling Logic --- */
    if(activeWindow == 1 || activeWindow == 2)
    {
        /* Windows 1 and 2 show Train Prefix on Line 1, and alternate between Values and Headers on Line 2 */
        line1Text[0] = (t->trainNumber / 10000) % 10 + '0';
        line1Text[1] = (t->trainNumber / 1000) % 10 + '0';
        line1Text[2] = (t->trainNumber / 100) % 10 + '0';
        line1Text[3] = (t->trainNumber / 10) % 10 + '0';
        line1Text[4] = t->trainNumber % 10 + '0';
        line1Text[5] = ' '; 

        if(stateStep == 0)
        {
            /* Train Parameter Values */
            i = 0;
            line2Text[i++] = t->platform + '0'; 
            line2Text[i++] = ' ';
            
            line2Text[i++] = (t->updatedArrivalHour / 10) + '0';
            line2Text[i++] = (t->updatedArrivalHour % 10) + '0';
            line2Text[i++] = ':';
            line2Text[i++] = (t->updatedArrivalMinute / 10) + '0';
            line2Text[i++] = (t->updatedArrivalMinute % 10) + '0';
            line2Text[i++] = ' ';

            line2Text[i++] = (t->updatedDepartureHour / 10) + '0';
            line2Text[i++] = (t->updatedDepartureHour % 10) + '0';
            line2Text[i++] = ':';
            line2Text[i++] = (t->updatedDepartureMinute / 10) + '0';
            line2Text[i++] = (t->updatedDepartureMinute % 10) + '0';
            line2Text[i++] = ' ';

            delayMins = t->delayMinutes; 
            line2Text[i++] = (delayMins / 10) + '0';
            line2Text[i++] = (delayMins % 10) + '0';
            
            while(i < 16) line2Text[i++] = ' ';
            line2Text[16] = '\0'; 
        }
        else
        {
            /* Column Headers */
            for(i = 0; i < 16; i++) line2Text[i] = headerText[i];
            line2Text[16] = '\0';
        }
    }
    else if(activeWindow == 3)
    {
        /* Window 3 logic: Before or at departure time, cycle Values -> Headers -> ARRIVED. 
           After departure time, display DEPARTED status. */
        line1Text[0] = (t->trainNumber / 10000) % 10 + '0';
        line1Text[1] = (t->trainNumber / 1000) % 10 + '0';
        line1Text[2] = (t->trainNumber / 100) % 10 + '0';
        line1Text[3] = (t->trainNumber / 10) % 10 + '0';
        line1Text[4] = t->trainNumber % 10 + '0';
        line1Text[5] = ' '; 

        if(currentTotalSecs > depTotalSecs)
        {
            /* After departure time: Force DEPARTED status frame for the 30-second post-departure window */
            stateStep = 3;
        }

        if(stateStep == 0)
        {
            /* Train Data Values */
            i = 0;
            line2Text[i++] = t->platform + '0'; line2Text[i++] = ' ';
            line2Text[i++] = (t->updatedArrivalHour / 10) + '0'; line2Text[i++] = (t->updatedArrivalHour % 10) + '0'; line2Text[i++] = ':';
            line2Text[i++] = (t->updatedArrivalMinute / 10) + '0'; line2Text[i++] = (t->updatedArrivalMinute % 10) + '0'; line2Text[i++] = ' ';
            line2Text[i++] = (t->updatedDepartureHour / 10) + '0'; line2Text[i++] = (t->updatedDepartureHour % 10) + '0'; line2Text[i++] = ':';
            line2Text[i++] = (t->updatedDepartureMinute / 10) + '0'; line2Text[i++] = (t->updatedDepartureMinute % 10) + '0'; line2Text[i++] = ' ';
            line2Text[i++] = (t->delayMinutes / 10) + '0'; line2Text[i++] = (t->delayMinutes % 10) + '0';
            
            while(i < 16) line2Text[i++] = ' ';
            line2Text[16] = '\0'; 
        }
        else if(stateStep == 1)
        {
            /* Header Display */
            for(i = 0; i < 16; i++) line2Text[i] = headerText[i];
            line2Text[16] = '\0';
        }
        else if(stateStep == 2)
        {
            /* ARRIVED status message with RTC time (less frequency/shorter duration as requested) */
            i = 0;
            line2Text[i++] = (HOUR / 10) + '0'; line2Text[i++] = (HOUR % 10) + '0'; line2Text[i++] = ':';
            line2Text[i++] = (MIN / 10) + '0';  line2Text[i++] = (MIN % 10) + '0';  line2Text[i++] = ':';
			line2Text[i++] = (SEC / 10) + '0';  line2Text[i++] = (SEC % 10) + '0';  line2Text[i++] = ' ';
            k = 0;
            while(arrStatus[k] != '\0') line2Text[i++] = arrStatus[k++];
            
            while(i < 16) line2Text[i++] = ' ';
            line2Text[16] = '\0';
        }
        else
        {
            /* DEPARTED status message with RTC time after departure */
            i = 0;
            line2Text[i++] = (HOUR / 10) + '0'; line2Text[i++] = (HOUR % 10) + '0'; line2Text[i++] = ':';
            line2Text[i++] = (MIN / 10) + '0';  line2Text[i++] = (MIN % 10) + '0';  line2Text[i++] = ' :';
			line2Text[i++] = (SEC / 10) + '0';  line2Text[i++] = (SEC % 10) + '0';  line2Text[i++] = ' ';
            k = 0;
            while(depStatus[k] != '\0') line2Text[i++] = depStatus[k++];
            
            while(i < 16) line2Text[i++] = ' ';
            line2Text[16] = '\0';
        }
    }

    /* Extract 10-Character Scrolling Window for Line 1 */
    for(j = 0; j < 10; j++)
    {
        u16 idxVal = (scrollPos1 + j) % len1;
        line1Text[6 + j] = scrollBuffer1[idxVal];
    }
    line1Text[16] = '\0';

    /* --- Safe Display Update on LCD --- */
    CmdLCD(GOTO_LINE1_POSO);
    StrLCD(line1Text);
    CmdLCD(GOTO_LINE2_POSO);
    StrLCD(line2Text);

    /* --- Timing and State Transition Control --- */
    scrollTimer++;
    if(scrollTimer >= 1) 
    {
        scrollTimer = 0;
        scrollPos1++;
        if(scrollPos1 >= len1) scrollPos1 = 0;

        stateTimer++;

        if(activeWindow < 3)
        {
            if(stateTimer >= 40) 
            {
                stateTimer = 0;
                stateStep = (stateStep + 1) % 2; 
            }
        }
        else
        {
            /* Window 3 timing control */
            u32 limit = 40;
            
            if(currentTotalSecs <= depTotalSecs)
            {
                /* Before/at departure time: cycle between Values (0), Headers (1), and ARRIVED (2) */
                if(stateStep == 0)      limit = 40;  /* Train Values duration */
                else if(stateStep == 1) limit = 25;  /* Headers duration */
                else if(stateStep == 2) limit = 20;  /* ARRIVED duration (less cycles/shorter as requested) */

                if(stateTimer >= limit)
                {
                    stateTimer = 0;
                    stateStep = (stateStep + 1) % 3; 
                }
            }
            else
            {
                /* After departure time: hold DEPARTED status (Step 3) */
                stateStep = 3;
            }
        }
    }
}
