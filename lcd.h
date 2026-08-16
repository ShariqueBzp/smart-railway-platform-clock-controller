/*****************************************************************************
 * @file      lcd.h
 * @brief     LCD Driver Header File for LPC2148 (8-bit Mode)
 * @details   Prototypes for LCD initialization, command/data writing, string 
 *            and number printing, and CGRAM custom character generation.
 *****************************************************************************/

#ifndef LCD_H
#define LCD_H

#include "types.h"

/* Function Prototypes */
void WriteLCD(u8 byte);
void CmdLCD(u8 opcode);
void InitLCD(void);
void CharLCD(u8 asciiVal);
void StrLCD(s8 *str);
void U32LCD(u32 num);
void S32LCD(s32 num);
void F32LCD(f32 fnum, u32 nDP);
void BuildCGRAM(s8 *p, u32 nBytes);

#endif /* LCD_H */
