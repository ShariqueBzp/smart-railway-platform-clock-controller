/*****************************************************************************
 * @file      kpm.h
 * @brief     Keypad Matrix Driver Header File for LPC2148
 * @details   Prototypes for keypad initialization, row/column scanning, 
 *            key detection, and numeric string parsing.
 *****************************************************************************/

#ifndef KPM_H
#define KPM_H

#include "types.h"

/* Function Prototypes */
void Init_KPM(void);
u32 RowCheck(void);
u32 ColCheck(void);
u8 KeyScan(void);
u32 ReadNum(void);
u8 ReadPass(void);

#endif /* KPM_H */
