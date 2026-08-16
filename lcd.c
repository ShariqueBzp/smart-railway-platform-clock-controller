/*****************************************************************************
 * @file      lcd.c
 * @brief     LCD Driver Source File for LPC2148 (8-bit Mode on Port 0)
 * @details   Implements low-level control functions for a 16x2 LCD interfaced
 *            in 8-bit mode on Port 0 (Data lines P0.6 to P0.13) using legacy
 *            GPIO registers and bitwise macros.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "delay.h"
#include "all_peripheral_defines.h"
#include "lcd.h"

/* Write a raw byte to the LCD data lines on Port 0 */
void WriteLCD(u8 byte) {
    /* Write byte to data pins using legacy byte-level macro on Port 0 */
    WRITEBYTE(IOPIN0, LCD_DATA, byte);

    /* Select write operation (RW = 0) */
    IOCLR0 = 1 << LCD_RW;

    /* Provide high-to-low enable pulse to latch data */
    IOSET0 = 1 << LCD_EN;
    delay_us(1);
    IOCLR0 = 1 << LCD_EN;
    delay_ms(2);
}

/* Send a command to the LCD instruction register */
void CmdLCD(u8 opcode) {
    /* Clear RS pin (0) for command register selection */
    IOCLR0 = 1 << LCD_RS;

    /* Write opcode via data pins */
    WriteLCD(opcode);
}

/* Initialize the LCD in 8-bit mode on Port 0 */
void InitLCD(void) {
    /* Configure P0.6-P0.13 (Data), RS, RW, and EN as GPIO output pins on Port 0 */
    IODIR0 |= ((0xFF << LCD_DATA) |
               (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_EN));

    delay_ms(15);
    CmdLCD(0x30);
    delay_ms(4);
    delay_us(100);
    CmdLCD(0x30);
    delay_us(100);
    CmdLCD(0x30);

    /* Configure operational modes and display settings */
    CmdLCD(MODE_8BIT_2LINE);
    CmdLCD(DSP_ON_CUR_OFF);
    CmdLCD(CLEAR_LCD);
    CmdLCD(SHIFT_CUR_RIGHT);
}

/* Send display data (character) to the LCD data register */
void CharLCD(u8 asciiVal) {
    /* Set RS pin (1) for data register selection */
    IOSET0 = 1 << LCD_RS;

    /* Write character byte to DDRAM via data pins */
    WriteLCD(asciiVal);
}

/* Print a null-terminated string to the LCD */
void StrLCD(s8 *str) {
    while (*str) {
        CharLCD(*str++);
    }
}

/* Print an unsigned 32-bit integer to the LCD */
void U32LCD(u32 num) {
    u8 a[10];
    s32 i = 0;
    if (num == 0) {
        CharLCD('0');
    } else {
        while (num > 0) {
            a[i++] = (num % 10) + 48;
            num /= 10;
        }
        for (--i; i >= 0; i--) {
            CharLCD(a[i]);
        }
    }
}

/* Print a signed 32-bit integer to the LCD */
void S32LCD(s32 num) {
    if (num < 0) {
        CharLCD('-');
        num = -num;
    }
    U32LCD((u32)num);
}

/* Print a floating-point number with specified decimal precision */
void F32LCD(f32 fnum, u32 nDP) {
    u32 num;
    s32 i;
    if (fnum < 0.0) {
        CharLCD('-');
        fnum = -fnum;
    }
    num = (u32)fnum;
    U32LCD(num);
    CharLCD('.');
    for (i = 0; i < nDP; i++) {
        fnum = (fnum - num) * 10.0;
        num = (u32)fnum;
        CharLCD(num + 48);
    }
}

/* Build Custom Characters in CGRAM */
void BuildCGRAM(s8 *p, u32 nBytes) {
    u32 i;
    /* Go to CGRAM start address */
    CmdLCD(GOTO_CGRAM_START);

    /* Select data register and write operation */
    IOSET0 = 1 << LCD_RS;
    IOCLR0 = 1 << LCD_RW;

    /* Write custom font pattern bytes to CGRAM */
    for (i = 0; i < nBytes; i++) {
        WriteLCD(p[i]);
    }

    /* Return back to standard DDRAM addressing */
    CmdLCD(GOTO_LINE1_POSO);
}
