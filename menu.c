/*****************************************************************************
 * @file      menu.c
 * @brief     Admin Menu Source File for LPC2148 Smart Railway Platform
 * @details   Implements password verification, strict 24-hour railway RTC time 
 *            configuration, date changes, password resets, and live train schedule/platform 
 *            and direct arrival/departure time editing.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "all_peripheral_defines.h"
#include "lcd.h"
#include "delay.h"
#include "kpm.h"
#include "rtc.h"
#include "trainDB.h"
#include "menu.h"

extern u8 save_pass;

/* Check password entered by user with C (Clear) and D (Backspace/Exit) handling */
u8 password(void)
{
    u8 temp;
    CmdLCD(0x01);
    StrLCD((s8 *)"Enter PIN:");
    CmdLCD(0xc0);
    temp = ReadPass();
    
    if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D')
    {
        return 'c';
    }
    if(temp == save_pass)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"Valid Password");
        delay_ms(500);
        return 1;
    }
    
    CmdLCD(0x01);
    StrLCD((s8 *)"Invalid Password");
    delay_ms(500);
    return 0;
}

/* Set RTC Time in Strict 24-Hour Railway Format (Hours: 0-23, Minutes: 0-59) */
void Time(void)
{
    u8 key;
    s32 temp;
    while(1)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"1.HOUR  2.MIN");
        CmdLCD(0xc0);
        StrLCD((s8 *)"3.EXIT  (D:Back)");
        
        key = KeyScan();
        switch(key)
        {
            case '1':
                CmdLCD(0x01);
                StrLCD((s8 *)"Hour (0-23):");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                
                if(temp < 0 || temp > 23)
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Hours!");
                    delay_ms(100);
                }
                else
                {
                    HOUR = (u8)temp;
                    SEC = 0;
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Hour Updated");
                    delay_ms(500);
                }
                break;
                
            case '2':
                CmdLCD(0x01);
                StrLCD((s8 *)"Min (0-59):");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                
                if(temp < 0 || temp > 59)
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Minutes!");
                    delay_ms(500);
                }
                else
                {
                    MIN = (u8)temp;
                    SEC = 0;
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Minute Updated");
                    delay_ms(500);
                }
                break;
                
            case '3':
            case 'D':
            case 'd':
            case 'c':
            case 'C':
                return;
        }
    }
}

/* Set RTC Date */
void DATE(void)
{
    u8 key;
    s32 temp;
    while(1)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"1.DATE  2.MONTH");
        CmdLCD(0xc0);
        StrLCD((s8 *)"3.YEAR  4.EXIT");
        
        key = KeyScan();
        switch(key)
        {
            case '1':
                CmdLCD(0x01);
                StrLCD((s8 *)"Date (1-31):");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                if(temp < 1 || temp > 31)
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Date!");
                    delay_ms(500);
                }
                else
                {
                    DOM = (u8)temp;
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Date Updated");
                    delay_ms(500);
                }
                break;
                
            case '2':
                CmdLCD(0x01);
                StrLCD((s8 *)"Month (1-12):");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                if(temp < 1 || temp > 12)
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Month!");
                    delay_ms(500);
                }
                else
                {
                    MONTH = (u8)temp;
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Month Updated");
                    delay_ms(500);
                }
                break;
                
            case '3':
                CmdLCD(0x01);
                StrLCD((s8 *)"Year:");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                if(temp < 2026 || temp > 4055)
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Year!");
                    delay_ms(500);
                }
                else
                {
                    YEAR = (u32)temp;
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Year Updated");
                    delay_ms(500);
                }
                break;
                
            case '4':
            case 'D':
            case 'd':
            case 'c':
            case 'C':
                return;
        }
    }
}

/* Set RTC Day of the Week */
void DAY(void)
{
    s32 temp;
    while(1)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"Day (0-6):");
        CmdLCD(0xc0);
        
        temp = ReadNum();
        if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') return;
        
        if(temp < 0 || temp > 6)
        {
            CmdLCD(0x01);
            StrLCD((s8 *)"Invalid Day!");
            delay_ms(500);
        }
        else
        {
            DOW = (u8)temp;
            CmdLCD(0x01);
            StrLCD((s8 *)"Day Updated");
            delay_ms(500);
            return;
        }
    }
}

/* Edit RTC Time and Date Menu */
void edit_time(void)
{
    u8 key;
    while(1)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"1.TIME  2.DATE");
        CmdLCD(0xc0);
        StrLCD((s8 *)"3.DAY  (D:Back)");
        
        key = KeyScan();
        switch(key)
        {
            case '1': Time(); break;
            case '2': DATE(); break;
            case '3': DAY(); break;
            case 'D':
            case 'd':
            case 'c':
            case 'C':
                return;
        }
    }
}

/* Edit Train Database Record Details (Platform, Delay, Arrival & Departure Times) */
void edit_train_details(void)
{
    u8 key;
    s32 temp;
    TrainInfo_t *selectedTrain = (TrainInfo_t*)0;
    unsigned char i;

    CmdLCD(0x01);
    StrLCD((s8 *)"Enter Train No:");
    CmdLCD(0xc0);
    temp = ReadNum();
    
    if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') return;

    for(i = 0; i < TOTAL_TRAINS; i++)
    {
        if(TrainDB[i].trainNumber == (unsigned int)temp)
        {
            selectedTrain = &TrainDB[i];
            break;
        }
    }

    if(selectedTrain == (TrainInfo_t*)0)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"Train Not Found!");
        delay_ms(1000);
        return;
    }

    /* Sub-menu handling 4 parameter options across multiple screens */
    while(1)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"1.PL 2.DLY 3.Arr");
        CmdLCD(0xc0);
        StrLCD((s8 *)"4.Dep  #:Save");
        
        key = KeyScan();
        switch(key)
        {
            case '1':
                CmdLCD(0x01);
                StrLCD((s8 *)"New Platform:");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                
                if(temp >= 1 && temp <= 9)
                {
                    selectedTrain->platform = (u8)temp;
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Plat Updated");
                    delay_ms(500);
                }
                else
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Platform");
                    delay_ms(500);
                }
                break;

            case '2':
                CmdLCD(0x01);
                StrLCD((s8 *)"Delay Minutes:");
                CmdLCD(0xc0);
                temp = ReadNum();
                if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') break;
                
                if(temp >= 0 && temp <= 180)
                {
                    unsigned int totalArrivalMins;
                    unsigned int totalDepartureMins;

                    selectedTrain->delayMinutes = (u8)temp;
                    
                    totalArrivalMins = (selectedTrain->arrivalHour * 60) + selectedTrain->arrivalMinute + temp;
                    selectedTrain->updatedArrivalHour = (totalArrivalMins / 60) % 24;
                    selectedTrain->updatedArrivalMinute = totalArrivalMins % 60;

                    totalDepartureMins = (selectedTrain->departureHour * 60) + selectedTrain->departureMinute + temp;
                    selectedTrain->updatedDepartureHour = (totalDepartureMins / 60) % 24;
                    selectedTrain->updatedDepartureMinute = totalDepartureMins % 60;

                    CmdLCD(0x01);
                    StrLCD((s8 *)"Delay Updated");
                    delay_ms(500);
                }
                else
                {
                    CmdLCD(0x01);
                    StrLCD((s8 *)"Invalid Delay");
                    delay_ms(500);
                }
                break;

            case '3': /* Direct Arrival Time Edit */
            {
                s32 arrH, arrM;
                CmdLCD(0x01);
                StrLCD((s8 *)"New Arr Hr(0-23)");
                CmdLCD(0xc0);
                arrH = ReadNum();
                if(arrH == 'c' || arrH == 'C' || arrH == 'd' || arrH == 'D') break;

                if(arrH < 0 || arrH > 23)
                {
                    CmdLCD(0x01); StrLCD((s8 *)"Invalid Hour!"); delay_ms(500);
                    break;
                }

                CmdLCD(0x01);
                StrLCD((s8 *)"New Ar Min(0-59)");
                CmdLCD(0xc0);
                arrM = ReadNum();
                if(arrM == 'c' || arrM == 'C' || arrM == 'd' || arrM == 'D') break;

                if(arrM < 0 || arrM > 59)
                {
                    CmdLCD(0x01); StrLCD((s8 *)"Invalid Min!"); delay_ms(500);
                    break;
                }

                selectedTrain->arrivalHour = (u8)arrH;
                selectedTrain->arrivalMinute = (u8)arrM;
                selectedTrain->updatedArrivalHour = (u8)arrH;
                selectedTrain->updatedArrivalMinute = (u8)arrM;

                CmdLCD(0x01);
                StrLCD((s8 *)"Arr Time Updated");
                delay_ms(500);
                break;
            }

            case '4': /* Direct Departure Time Edit */
            {
                s32 depH, depM;
                CmdLCD(0x01);
                StrLCD((s8 *)"New Dp Hr(0-23)");
                CmdLCD(0xc0);
                depH = ReadNum();
                if(depH == 'c' || depH == 'C' || depH == 'd' || depH == 'D') break;

                if(depH < 0 || depH > 23)
                {
                    CmdLCD(0x01); StrLCD((s8 *)"Invalid Hour!"); delay_ms(500);
                    break;
                }

                CmdLCD(0x01);
                StrLCD((s8 *)"New Dp Min(0-59)");
                CmdLCD(0xc0);
                depM = ReadNum();
                if(depM == 'c' || depM == 'C' || depM == 'd' || depM == 'D') break;

                if(depM < 0 || depM > 59)
                {
                    CmdLCD(0x01); StrLCD((s8 *)"Invalid Min!"); delay_ms(500);
                    break;
                }

                selectedTrain->departureHour = (u8)depH;
                selectedTrain->departureMinute = (u8)depM;
                selectedTrain->updatedDepartureHour = (u8)depH;
                selectedTrain->updatedDepartureMinute = (u8)depM;

                CmdLCD(0x01);
                StrLCD((s8 *)"Dep Time Updated");
                delay_ms(500);
                break;
            }

            case '#':
                CmdLCD(0x01);
                StrLCD((s8 *)"Saved Success!");
                delay_ms(500);
                return;

            case 'D':
            case 'd':
            case 'c':
            case 'C':
                return;
        }
    }
}

/* Edit and Update Admin Password (PIN) */
void edit_password(void)
{
    u8 newPass, confirmPass;

    CmdLCD(0x01);
    StrLCD((s8 *)"New PIN:");
    CmdLCD(0xc0);
    newPass = ReadPass();
    
    if(newPass == 'c' || newPass == 'C' || newPass == 'd' || newPass == 'D') return;

    CmdLCD(0x01);
    StrLCD((s8 *)"Confirm PIN:");
    CmdLCD(0xc0);
    confirmPass = ReadPass();
    
    if(confirmPass == 'c' || confirmPass == 'C' || confirmPass == 'd' || confirmPass == 'D') return;

    if(newPass == confirmPass)
    {
        save_pass = newPass;
        CmdLCD(0x01);
        StrLCD((s8 *)"Password Updated");
        delay_ms(500);
    }
    else
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"Pins Don't Match");
        delay_ms(500);
    }
}

/* Admin Dashboard invoked when EINT1 sets admin_flag = 1 */
void RunAdminDashboard(void)
{
    u8 key, temp;
    s8 count = 3;
    
    do {
        temp = password();
        if(temp == 'c' || temp == 'C' || temp == 'd' || temp == 'D') return;
        if(temp == 1) break;
        
        if(temp == 0)
        {
            CmdLCD(0x01);
            count--;
            StrLCD((s8 *)"Attempts left:");
            CmdLCD(0xc0);
            U32LCD((u32)count);
            StrLCD((s8 *)" Wait 1s");
            delay_ms(1000);
            if(count == 0) return;
        }
    } while(count);

    while(1)
    {
        CmdLCD(0x01);
        StrLCD((s8 *)"A.RTC   B.TrainDB");
        CmdLCD(0xc0);
        StrLCD((s8 *)"C.Pass  D.Exit");
        
        key = KeyScan();
        switch(key)
        {
            case 'A':
            case 'a': edit_time(); break;
            case 'B':
            case 'b': edit_train_details(); break;
            case 'C':
            case 'c': edit_password(); break;
            case 'D':
            case 'd': 
                CmdLCD(0x01);
                StrLCD("Exiting...");
                delay_ms(500);
                return;
        }
    }
}