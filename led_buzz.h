/*****************************************************************************
 * @file      led_buzz.h
 * @brief     LED and Buzzer Control Header File for LPC2148
 * @details   Prototypes for status indicators (On-Time, Approaching, Delayed LEDs) 
 *            and the warning buzzer using Port 1 Fast GPIO pins.
 *****************************************************************************/

#ifndef LED_BUZZ_H
#define LED_BUZZ_H

#include "types.h"

/* Function Prototypes */
void LED_Buzz_Init(void);
void LED_OnTime_Ctrl(u8 state);
void LED_Approach_Ctrl(u8 state);
void LED_Delayed_Ctrl(u8 state);
void Buzzer_Ctrl(u8 state);

#endif /* LED_BUZZ_H */
