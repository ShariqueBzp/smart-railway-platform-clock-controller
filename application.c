/*****************************************************************************
 * @file     application.c
 * @brief    Main Application Source File for LPC2148 Smart Railway Platform
 * @details  Implements core execution workflow combining initialization,
 *           dual-mode LCD display management (Static RTC vs. Train DB), 
 *           real-time RTC and TrainDB comparisons, LED/buzzer indicator actuation, 
 *           and external interrupt-driven admin configuration mode.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "all_peripheral_defines.h"
#include "lcd.h"
#include "delay.h"
#include "rtc.h"
#include "kpm.h"
#include "led_buzz.h"
#include "eint1_sw.h"
#include "scheduler.h"
#include "menu.h"
#include "trainDB.h"

/* Global definition of admin password PIN */
u8 save_pass = 1234;

/* External volatile flag from interrupt handler */
extern volatile u8 admin_flag;

int main(void)
{
    u32 displayTimer = 0;
    u8 lcdMode = 0; /* 0: Default Static RTC View, 1: Train Announcement View */
    u8 lastMode = 0xFF; /* Track mode changes to handle screen refreshes cleanly */
    u8 i;
    u8 hasDelay;
    u8 hasArrival;
    
    s32 curHour, curMin, curSec;
    s32 curDate, curMonth, curYear;
    s32 curDay;

    /* [ POWER ON / RESET ] & [ Initialize Hardware Peripherals ] */
    RTC_Init();
	  InitLCD();
	  LED_Buzz_Init();
	  Init_KPM();
	  Init_Scheduler();
    Init_EINT1();

    /* Startup Splash Screen */
    CmdLCD(0x01);
    StrLCD((s8 *)"--WeLCome To--");
    CmdLCD(0xc0);
    StrLCD((s8 *)"Indian Railways");
    delay_ms(2000);

    /* MAIN LOOP */
    while(1)
    {
        /* [ Check Admin Interrupt Occurred (Admin Switch Pressed) ] */
        if(admin_flag)
        {
            admin_flag = 0;             /* Clear interrupt flag */
            RunAdminDashboard();        /* Enters Admin Dashboard */
            displayTimer = 0;           /* Reset display timer on exit */
            lastMode = 0xFF;            /* Force screen refresh after admin menu */
        }

        /* Fetch current time, date, and day using rtc.h getters */
        GetRTCTimeInfo(&curHour, &curMin, &curSec);
        GetRTCDateInfo(&curDate, &curMonth, &curYear);
        GetRTCDay(&curDay);

        /* [ Check Train Schedules ] */
        CheckTrainSchedules();

        /* Evaluate train statuses and drive RGY LEDs & Buzzer */
        hasDelay = 0;
        hasArrival = 0;

        for(i = 0; i < TOTAL_TRAINS; i++)
        {
            if(TrainDB[i].delayMinutes > 0)
            {
                hasDelay = 1;
            }
            if(TrainDB[i].trainStatus == STATUS_ARRIVED)
            {
                hasArrival = 1;
            }
        }

        /* Update indicator states */
        LED_Delayed_Ctrl(hasDelay);
        LED_Approach_Ctrl(hasArrival);
        LED_OnTime_Ctrl(!hasDelay);
        
        /* Audible warning pulse on buzzer if a train is currently on platform */
        
				
				
				
				if(hasArrival)
					{
						
            Buzzer_Ctrl(1);
            delay_ms(100);
            Buzzer_Ctrl(0);
					}

        /* [ Dual-Mode LCD Management & Status Alternation ] */
        displayTimer++;
        if(displayTimer < 60)        /* Mode 0 duration window (Static RTC) */
        {
            lcdMode = 0;
        }
        else if(displayTimer < 120) /* Mode 1 duration window (Train Info) */
        {
            lcdMode = 1;
        }
        else
        {
            displayTimer = 0;
        }

        /* Clear screen only when switching modes to prevent flickering */
        if(lcdMode != lastMode)
        {
            CmdLCD(0x01);
            lastMode = lcdMode;
        }

        if(lcdMode == 0)
        {
            /* Mode 0: Default Static Real-Time Clock View (Overwriting without clearing) */
            CmdLCD(GOTO_LINE1_POSO);
            
            /* Line 1: HH:MM:SS DAY */
            CharLCD((curHour / 10) + '0');
            CharLCD((curHour % 10) + '0');
            CharLCD(':');
            CharLCD((curMin / 10) + '0');
            CharLCD((curMin % 10) + '0');
            CharLCD(':');
            CharLCD((curSec / 10) + '0');
            CharLCD((curSec % 10) + '0');
            CharLCD(' ');
            
            /* Print Day string using week array from rtc.h */
            if(curDay >= 0 && curDay < 7)
            {
                StrLCD(week[curDay]);
            }
            else
            {
                StrLCD((s8 *)"---");
            }

            /* Line 2: DD/MM/YYYY */
            CmdLCD(GOTO_LINE2_POSO);
            CharLCD((curDate / 10) + '0');
            CharLCD((curDate % 10) + '0');
            CharLCD('/');
            CharLCD((curMonth / 10) + '0');
            CharLCD((curMonth % 10) + '0');
            CharLCD('/');
            U32LCD(curYear);
        }
        else
        {
            /* Mode 1: Train Announcement View */
            DisplayActiveTrainStatus();
        }

        /* Balanced execution tick delay */
        delay_ms(200);
    }
}
