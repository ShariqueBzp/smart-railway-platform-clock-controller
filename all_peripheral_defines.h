/*****************************************************************************
 * @file      all_peripheral_defines.h
 * @brief     Consolidated Peripheral Definitions and Bit Manipulation Macros
 *            for LPC2148 Smart Railway Platform Controller
 * @details   Synchronized production-grade header using validated pin mappings:
 *            - 4x4 Keypad  : Port 1 (Rows on P1.16-P1.19, Cols on P1.20-P1.23)
 *            - LCD Data Bus : Port 0 (P0.6 to P0.13)
 *            - LCD Control  : RS on P0.17, EN on P0.18, RW on P0.19
 *            - Admin Switch : P0.3 (EINT1)
 *            - Status LEDs  : P1.27, P1.28, P1.29
 *            - Buzzer       : P1.26
 *****************************************************************************/

#ifndef ALL_PERIPHERAL_DEFINES_H
#define ALL_PERIPHERAL_DEFINES_H

/* ================================================================= */
/* 1. STANDARD INCLUDES & DEPENDENCIES                             */
/* ================================================================= */
#include <LPC21xx.h>
#include <stdlib.h>
#include "types.h"

/* ================================================================= */
/* 2. BIT MANIPULATION & REGISTER HELPER MACROS                      */
/* ================================================================= */
#define SETBIT(WORD, BIT)               ((WORD) |=  (1UL << (BIT)))
#define CLRBIT(WORD, BIT)               ((WORD) &= ~(1UL << (BIT)))
#define CPLBIT(WORD, BIT)               ((WORD) ^=  (1UL << (BIT)))
#define WRITEBIT(WORD, BITPOS, LEVEL)   ((WORD) =   ((WORD) & ~(1UL << (BITPOS))) | ((LEVEL) << (BITPOS)))
#define READBIT(WORD, BITPOS)           (((WORD) >> (BITPOS)) & 1UL)

#define READWRITEBIT(DWORD, DBIT, SWORD, SBIT) \
    ((DWORD) = ((DWORD) & ~(1UL << (DBIT))) | ((((SWORD) >> (SBIT)) & 1UL) << (DBIT)))

#define WRITENIBBLE(WORD, POS, VAL)     ((WORD) = ((WORD) & ~(15UL << (POS))) | (((VAL) & 0x0F) << (POS)))
#define READNIBBLE(WORD, POS)           (((WORD) >> (POS)) & 0x0F)

#define WRITEBYTE(WORD, POS, BYTE)      ((WORD) = ((WORD) & ~(0xFF << (POS))) | (((BYTE) & 0xFF) << (POS)))
#define READBYTE(WORD, POS)             (((WORD) >> (POS)) & 0xFF)

#define WRITENBITS(WORD, POS, NBITS, VAL) \
    ((WORD) = ((WORD) & ~(((1UL << (NBITS)) - 1UL) << (POS))) | (((VAL) & ((1UL << (NBITS)) - 1UL)) << (POS)))

/* ================================================================= */
/* 3. SYSTEM CLOCK & RTC PRESCALER CONFIGURATION                     */
/* ================================================================= */
#define F_OSC           12000000UL      /* Oscillator Frequency: 12 MHz       */
#define SYS_CCLK        (5 * F_OSC)     /* Core Clock: 60 MHz                 */
#define SYS_PCLK        (SYS_CCLK / 4)  /* Peripheral Clock: 15 MHz           */

#define PCLK1           15000000UL      /* Dedicated RTC Peripheral Clock     */
#define PREINT_VALUE    ((PCLK1 / 32768) - 1)
#define PREFRAC_VALUE   (PCLK1 - ((PREINT_VALUE + 1) * 32768))

/* ================================================================= */
/* 4. LCD CONFIGURATION (PORT 0 INTERFACE)                           */
/* ================================================================= */
#define LCD_DATA        6               /* Data lines: P0.6 to P0.13          */
#define LCD_RS          17              /* Register Select pin: P0.17         */
#define LCD_EN          18              /* Enable pin: P0.18                  */
#define LCD_RW          19              /* Read/Write pin: P0.19              */

/* --- Standard LCD Command Definitions --- */
#define CLEAR_LCD           0X01
#define RET_CUR_HOME        0X02
#define LCD_RESET           0X03
#define SHIFT_CUR_RIGHT     0X06
#define SHIFT_CUR_LEFT      0X07
#define DSP_OFF             0X08
#define DSP_ON_CUR_OFF      0X0C
#define DSP_ON_CUR_ON       0X0E
#define DSP_ON_CUR_BLK      0X0F
#define SHIFT_DSP_LEFT      0X10
#define SHIFT_DSP_RIGHT     0X14
#define MODE_8BIT_1LINE     0X30
#define MODE_4BIT_1LINE     0X20
#define MODE_8BIT_2LINE     0X38
#define MODE_4BIT_2LINE     0X28

#define GOTO_LINE1_POSO     0X80
#define GOTO_LINE2_POSO     0XC0
#define GOTO_LINE3_POSO     0X90
#define GOTO_LINE4_POSO     0XD0
#define GOTO_CGRAM_START    0X40

/* ================================================================= */
/* 5. EXTERNAL INTERRUPT (EINT1) ADMIN SWITCH                        */
/* ================================================================= */
#define ADMIN_EINT_PIN      3           /* Admin Switch on P0.3 (EINT1)       */

/* ================================================================= */
/* 6. 4x4 MATRIX KEYPAD CONFIGURATION (PORT 1 INTERFACE)             */
/* ================================================================= */
#define ROW0     16
#define ROW1     17
#define ROW2     18
#define ROW3     19

#define COL0     20
#define COL1     21
#define COL2     22
#define COL3     23

/* ================================================================= */
/* 7. STATUS INDICATORS (LEDS & BUZZER) - PORT 1                     */
/* ================================================================= */
#define BUZZER_PIN      26              /* Buzzer control pin: P1.26          */
#define LED_ON_TIME     27              /* Green LED: On-Time Status (P1.27)  */
#define LED_APPROACH    28              /* Yellow LED: Approaching (P1.28)    */
#define LED_DELAYED     29              /* Red LED: Delayed Status (P1.29)    */

#endif /* ALL_PERIPHERAL_DEFINES_H */