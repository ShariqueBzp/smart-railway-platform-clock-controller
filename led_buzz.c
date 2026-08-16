/*****************************************************************************
 * @file      led_buzz.c
 * @brief     LED and Buzzer Control Source File for LPC2148
 * @details   Implements hardware initialization and control functions for status 
 *            LEDs (P1.27, P1.28, P1.29) and the buzzer (P1.26) using Fast GPIO.
 *            Note: Fast GPIO (FIODIR/FIOSET/FIOCLR) is used instead of legacy 
 *            IODIR/IOPIN for single-cycle execution speed and safe atomic bit manipulation.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "all_peripheral_defines.h"
#include "led_buzz.h"

/* Initialize LED and Buzzer pins as GPIO outputs */
void LED_Buzz_Init(void)
{
    /* Set direction for LED pins and Buzzer pin on Port 1 as output (1) */
    IODIR1 |= (1UL << LED_ON_TIME) | 
               (1UL << LED_APPROACH) | 
               (1UL << LED_DELAYED)  | 
               (1UL << BUZZER_PIN);

    /* Initialize all indicators to OFF state */
    IOCLR1 = (1UL << LED_ON_TIME) | 
              (1UL << LED_APPROACH) | 
              (1UL << LED_DELAYED)  | 
              (1UL << BUZZER_PIN);
}

/* Control Green LED (On-Time Status) */
void LED_OnTime_Ctrl(u8 state)
{
    if (state)
		{
        IOSET1 = (1UL << LED_ON_TIME);   
		}
	 else
	 {
        IOCLR1 = (1UL << LED_ON_TIME);
   }
}

/* Control Yellow LED (Approaching Status) */
void LED_Approach_Ctrl(u8 state)
{
    if (state)
		{
        IOSET1 = (1UL << LED_APPROACH);
		}
	else
	{
        IOCLR1 = (1UL << LED_APPROACH);
  }
}

/* Control Red LED (Delayed Status) */
void LED_Delayed_Ctrl(u8 state)
{
    if (state)
	{
        IOSET1 = (1UL << LED_DELAYED);
  }
	else 
	{
        IOCLR1 = (1UL << LED_DELAYED);
  }
}

/* Control Warning Buzzer */
void Buzzer_Ctrl(u8 state)
{
    if (state)
	{
        IOSET1 = (1UL << BUZZER_PIN);
  } 
	else
	{
        IOCLR1 = (1UL << BUZZER_PIN);
  }
}
